#include "processes.h"

void dispatcher_process(shared_t *shm, semaphores_t *sem, FILE *file, int O)
{
	print_action(shm, file, "D: started\n");

	while(true)
	{
		//find out if there are any visitors left
		sem_wait(&sem->mutex);
		/* INSIDE MUTEX */
		if (shm->visitors_left == 0)
		{	
			print_action(shm, file, "D: closing\n");
			shm->closing = true; //let carts know we're closing
			sem_post(&sem->mutex);
			sem_post(&sem->cart_in);
			break;
		}

		print_action(shm, file, "D: next cart\n");
		/* EXITTING MUTEX */
		sem_post(&sem->mutex);

		//send cue to cart to get int
		sem_post(&sem->cart_in);

		//wait until cart leaves
		sem_wait(&sem->cart_leaving);

		//wait for Omics
		usleep(O);
	}
	
	//check for being the last process
	sem_wait(&sem->mutex);
	/* INSIDE MUTEX */
	shm->forked_count--;
	if (shm->forked_count == 0)
		sem_post(&sem->all_finished);
	/* EXITTING MUTEX */
	sem_post(&sem->mutex);

	exit(0);
}

void cart_process(shared_t *shm, semaphores_t *sem, FILE *file, int v_id, int TV, int cart_capacity)
{
	sem_wait(&sem->mutex);
	print_action(shm, file, "V %d: started\n", v_id);
	sem_post(&sem->mutex);

	while (true)
	{	
		//wait for cart_in cue
		sem_wait(&sem->cart_in);

		sem_wait(&sem->mutex);
		/* INSIDE MUTEX */

		//if closing, end the process
		if (shm->closing)
		{
			sem_post(&sem->cart_in); //keep cart_in open for carts that haven't closed yet
			print_action(shm, file, "V %d: closed\n", v_id);
			sem_post(&sem->mutex);
			break;
		}
		//determine how many people to board
		int visitors_taken =     shm->visitors_left < cart_capacity ?
                                 shm->visitors_left :
                                 cart_capacity;
		//put own id to cart_boarding_id for visitors to know which cart they're in
		shm->boarding_cart_id = v_id;
		shm->to_be_boarded = visitors_taken;
		
		print_action(shm, file, "V %d: boarding started\n", v_id);
		/* EXITTING MUTEX */
		sem_post(&sem->mutex);

		//boarding started

		//signal for visitors to get in
		sem_post(&sem->cart_boarding);
		
		//wait until cart is full (signal from last visitor to board)
		sem_wait(&sem->cart_full);

		sem_wait(&sem->mutex);
		print_action(shm, file, "V %d: boarding complete\n", v_id);
		sem_post(&sem->mutex);

		//notify dispatcher about departure
		sem_post(&sem->cart_leaving);

		//wait for TV/2ms and wait to be the only cart in the destination (TODO rand)
		usleep(TV/2);
		sem_wait(&sem->cart_emptying);

		//put id of cart that is to be emptied to shared memory (so that visitors know if they should leave)
		sem_wait(&sem->mutex);
		/* INSIDE MUTEX */
		shm->leaving_cart_id = v_id;
		shm->to_leave_cart = visitors_taken;
		
		print_action(shm, file, "V %d: leaving started\n", v_id);
		/* LEAVING MUTEX */
		sem_post(&sem->mutex);

		//tell visitors to leave
		sem_post(&sem->leave_cart);

		//wait until cart is empty (cue from the last visitor to leave)
		sem_wait(&sem->cart_emptied);
		sem_post(&sem->cart_emptying);

		sem_wait(&sem->mutex);
		print_action(shm, file, "V %d: leaving complete\n", v_id);
		sem_post(&sem->mutex);
	}

	//check for being the last process
	sem_wait(&sem->mutex);
	/* INSIDE MUTEX */
	shm->forked_count--;
	if (shm->forked_count == 0)
		sem_post(&sem->all_finished);
	/* EXITTING MUTEX */
	sem_post(&sem->mutex);

	exit(0);
}

void visitor_process(shared_t *shm, semaphores_t *sem, FILE *file, int n_id, int TN)
{
	sem_wait(&sem->mutex);
	print_action(shm, file, "N %d: started\n", n_id);
	sem_post(&sem->mutex);

	//wait in given interval
	usleep(TN);	//TODO rand

	sem_wait(&sem->mutex);
	print_action(shm, file, "N %d: queue\n", n_id);
	sem_post(&sem->mutex);

	//wait for boarding
	sem_wait(&sem->cart_boarding);
	/* BOARDING */
	sem_wait(&sem->mutex);
	/* INSIDE MUTEX */
	print_action(shm, file, "N %d: boarding\n", n_id);
	
	//get cart ID, remember it
	int my_cart = shm->boarding_cart_id;
	shm->to_be_boarded--;
	shm->visitors_left--; //signalize that this visitor has been resolved
	if (shm->to_be_boarded == 0) //last to be boarder
		sem_post(&sem->cart_full); //let cart know it can go
	else
		sem_post(&sem->cart_boarding); //let others also board
	/* EXIT MUTEX */
	sem_post(&sem->mutex);

	/* BOARDED */

	//wait for leaving, but also check if leaving is meant for 'my_cart'
	while (true)
	{
		sem_wait(&sem->leave_cart);
		
		sem_wait(&sem->mutex);
		/* INSIDE MUTEX */
		//check id
		if (shm->leaving_cart_id != my_cart) //if !match, not a signal for this visitor
		{
			sem_post(&sem->mutex);
			sem_post(&sem->leave_cart);
			continue;
		}
		shm->to_leave_cart--; //signalize leaving the cart
		int curr_cart_visitors = shm->to_leave_cart;
		print_action(shm, file, "N %d: leaving\n", n_id);
		/* EXIT MUTEX */
		sem_post(&sem->mutex);
		if (curr_cart_visitors == 0)
			sem_post(&sem->cart_emptied); //case last to leave
		else
			sem_post(&sem->leave_cart); //else also let others leave

		break; //if here, resolved
	}

	//check for being the last process
	sem_wait(&sem->mutex);
	/* INSIDE MUTEX */
	shm->forked_count--;
	if (shm->forked_count == 0)
		sem_post(&sem->all_finished);
	/* EXITTING MUTEX */
	sem_post(&sem->mutex);

	exit(0);
}

#include <unistd.h>
#include "semaphore.h"
#include "shared_mem.h"

void visitor_process(shared_t *shm, semaphores_t *sem, FILE *file, int n_id, int TN)
{
	print_action(shm, sem->mutex, file, "N %d: started\n", n_id);

	//wait in given interval
	usleep(TN);	//TODO rand

	print_action(shm, sem->mutex, file, "N %d: queue\n", n_id);

	//wait for boarding
	sem_wait(sem->cart_boarding);
	/* BOARDING */
	print_action(shm, sem->mutex, file, "N %d: boarding\n", n_id);
	
	sem_wait(sem->mutex);
	/* INSIDE MUTEX */
	//get cart ID, remember it
	int my_cart = shm->boarding_cart_id;
	shm->to_be_boarded--;
	shm->visitors_left--; //signalize that this visitor has been resolved
	if (shm->to_be_boarded == 0) //last to be boarder
		sem_post(sem->cart_full); //let cart know it can go
	else
		sem_post(sem->cart_boarding); //let others also board
	/* EXIT MUTEX */
	sem_post(sem->mutex);

	/* BOARDED */

	//wait for leaving, but also check if leaving is meant for 'my_cart'
	while (true)
	{
		sem_wait(sem->leave_cart);
		
		sem_wait(sem->mutex);
		/* INSIDE MUTEX */
		//check id
		if (shm->leaving_cart_id != my_cart) //if !match, not a signal for this visitor
		{
			sem_post(sem->mutex);
			sem_post(sem->leave_cart);
			continue;
		}
		shm->to_leave_cart--; //signalize leaving the cart
		curr_cart_visitors = shm->to_leave_cart;
		/* EXIT MUTEX */
		sem_post(sem->mutex);
		print_action(shm, sem->mutex, file, "N %d: leaving\n", n_id);
		if (curr_cart_visitors == 0)
			sem_post(sem->cart_emptied); //case last to leave
		else
			sem_post(sem->leave_cart); //else also let others leave

		break; //if here, resolved
	}

	//check for being the last process
	sem_wait(sem->mutex);
	/* INSIDE MUTEX */
	shm->forked_count--;
	if (shm->forked_count == 0)
		sem_post(all_finished);
	/* EXITTING MUTEX */
	sem_post(sem->mutex);

	exit(0);
}



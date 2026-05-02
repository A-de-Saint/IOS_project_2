#include "semaphores.h"
#include "shared_mem.h"
#include <unistd.h>
#include <stdlib.h>

void cart_process(shared_t *shm, semaphores_t *sem, FILE *file, int v_id, int TV, int cart_capacity)
{
	print_action(shm, sem->mutex, file, "V %d: started\n", v_id);
	
	//TODO probably will need to put another identificator into the shm->cart_*_id, because the same cart might go multiple times
	//Although it might not happen, due to diffenent waiting
	//Think about this is the only TODO in the end

	while (true)
	{	
		//wait for cart_in cue
		sem_wait(sem->cart_in);

		sem->wait(sem->mutex);
		/* INSIDE MUTEX */

		//if closing, end the process
		if (shm->closing)
		{
			sem_post(sem->mutex);
			sem_post(cart_in); //keep cart_in open for carts that haven't closed yet
			print_action(shm, sem->mutex, file, "V %d: closed\n", v_id);
			break;
		}
		//determine how many people to board
		int visitors_taken =     shm->visitors_left < cart_capacity ?
                                 shm->visitors_left :
                                 shm->cart_capacity;
		else //put own id to cart_boarding_id for visitors to know which cart they're in
		{
			shm->boarding_cart_id = v_id;
			shm->to_be_boarded = visitors_taken;
		}
		/* EXITTING MUTEX */
		sem_post(sem->mutex);
		
		//boarding here
		print_action(shm, sem->mutex, file, "V %d: boarding started\n", v_id);

		//signal for visitors to get in
		sem_post(sem->cart_boarding);
		
		//wait until cart is full (signal from last visitor to board)
		sem_wait(sem->cart_full);
		print_action(shm, sem->mutex, file, "V %d: boarding complete\n", v_id);

		//wait for TV/2ms and wait to be the only cart in the destination (TODO rand)
		usleep(TV/2);
		sem_wait(sem->cart_emptying);

		//put id of cart that is to be emptied to shared memory (so that visitors know if they should leave)
		sem_wait(sem->mutex);
		/* INSIDE MUTEX */
		shm->leaving_cart_id = v_id;
		shm->to_leave_cart = visitors_taken;
		/* LEAVING MUTEX */
		sem_post(sem->mutex);
		
		print_action(shm, sem->mutex, file, "V %d: leaving started\n", v_id);

		//tell visitors to leave
		sem_post(sem->leave_cart);

		//wait until cart is empty (cue from the last visitor to leave)
		sem_wait(sem->cart_emptied);
		sem_post(sem->cart_emptying);

		print_action(shm, sem->mutex, file, "V %d: leaving complete\n", v_id);
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



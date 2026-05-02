#include "output.h"
#include "semaphores.h"
#include <unistd.h>

void dispatcher_process(shared_t *shm, semaphores_t *sem, FILE *file, int O)
{
	print_action(shm, sem->mutex, file, "D: started\n");

	while(true)
	{
		//find out if there are any visitors left
		sem_wait(mutex);
		/* INSIDE MUTEX */
		if (shm->visitors_left == 0)
		{	
			sem_post(mutex);
			print_action(shm, sem->mutex, file, "D: closing\n");
			sem_post(sem->closing);
			sem_post(sem->cart_in);
			break;
		}
		/* EXITTING MUTEX */
		sem_post(mutex);

		//send cue to cart to get int
		print_action(shm, mutex, file, "D: next cart");
		sem_post(cart_in);

		//wait until cart leaves
		sem_wait(cart_leaving);

		//wait for Omics
		usleep(O);
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


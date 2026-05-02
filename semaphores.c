#include "semaphores.h"

semaphores_t *semaphores_init()
{	
	//map shared memory - not a file, 0 offset
	semaphores_t *sems = mmap(NULL, sizeof(semaphores_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (sems == MAP_FAILED) //check if mmap was successful
		return NULL;

	/*
	 	The next part inits semaphores - if any initialization fails, it jumps to the proper label
		and destroys all previously init'd semaphores and unmaps sems
	*/

	//mutex starts green
	if (sem_init(&sems->mutex, 1, 1) != 0)
		goto mutex_fail;

	//cart_in starts red
	if (sem_init(&sems->cart_in, 1, 0) != 0)
		goto cart_in_fail;
	
	//cart_boarding starts red
	if (sem_init(&sems->cart_boarding, 1, 0) != 0)
		goto cart_boarding_fail;

	//cart_full starts red
	if (sem_init(&sems->cart_full, 1, 0) != 0)
		goto cart_full_fail;
	
	//cart_leaving starts red
	if (sem_init(&sems->cart_leaving, 1, 0) != 0)
		goto cart_leaving_fail;

	//cart_emptying starts green
	if (sem_init(&sems->cart_emptying, 1, 1) != 0)
		goto cart_emptying_fail;

	//leave_cart starts red
	if (sem_init(&sems->leave_cart, 1, 0) != 0)
		goto leave_cart_fail;

	//cart_emptied starts red
	if (sem_init(&sems->cart_emptied, 1, 0) != 0)
		goto cart_emptied_fail;

	//all_finished starts red
	if (sem_init(&sems->all_finished, 1, 0) != 0)
		goto all_finished_fail;

	return sems;

	//A set of destructors in case initialization fails at any point
  all_finished_fail:
	sem_destroy(&sems->cart_emptied);
  cart_emptied_fail:
	sem_destroy(&sems->leave_cart);
  leave_cart_fail:
	sem_destroy(&sems->cart_emptying);
  cart_emptying_fail:
	sem_destroy(&sems->cart_leaving);
  cart_leaving_fail:
	sem_destroy(&sems->cart_full);
  cart_full_fail:
	sem_destroy(&sems->cart_boarding);
  cart_boarding_fail:
	sem_destroy(&sems->cart_in);
  cart_in_fail:
  	sem_destroy(&sems->mutex);
  mutex_fail:

	munmap(sems, sizeof(semaphores_t));

  	return NULL;
}


void semaphores_dtor(semaphores_t *sems)
{
	//destroy all semaphores
	sem_destroy(&sems->mutex);
	sem_destroy(&sems->cart_in);
	sem_destroy(&sems->cart_boarding);
	sem_destroy(&sems->cart_full);
	sem_destroy(&sems->cart_leaving);
	sem_destroy(&sems->cart_emptying);
	sem_destroy(&sems->leave_cart);
	sem_destroy(&sems->cart_emptied);

	//unmap
	munmap(sems, sizeof(semaphores_t));
}


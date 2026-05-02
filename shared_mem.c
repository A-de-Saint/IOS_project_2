#include "shared_mem.h"
#include <stdlib.h>

shared_t *shared_mem_init(int visitors_N, int forked_count)
{	
	//shared memory allocation - not a file, 0 offset
	shared_t *shm = mmap(NULL, sizeof(shared_t), MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (shm == MAP_FAILED)
		return NULL;

	//initialize values
	shm->action_counter = 1;
	shm->visitors_left = visitors_N;
	shm->closing = false;
	shm->forked_processes = forked_count;

	return shm;
}


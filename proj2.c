#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include "argument_parser.h"
#include "semaphores.h"

#define OUTPUT_FILENAME "proj2.out"

int main(int argc, char **argv)
{
	//get arguments
	args_t args;
	if (argc < 2 || !parse_args(argc - 1, &argv[1], &args))
	{
		fprintf(stderr, "Could not parse arguments\n");
		return 1;
	}

	//get semaphores
	semaphores_t *sems = semaphores_init();
	if (sems == NULL)
	{
		fprintf(stderr, "Could not map or allocate needed semaphores\n");
		return 1;
	}

	int total_processes = 1 + args.N + args.V;

	//get shared memory
	shared_t *shm = shared_mem_init(args.N, total_processes);
	if (shm == NULL)
	{
		fprintf(stderr, "Could not map needed shared memory\n");
		semaphores_dtor(sems);
		return 1;
	}

	//open file
	FILE *file = fopen(OUTPUT_FILENAME, "w");
	if (file == NULL)
	{
		fprintf(stderr, "Could not open %s for write\n", OUTPUT_FILENAME);
		semaphores_dtor(sems);
		shared_dtor(shm);
		return 1;
	}
	
	//group processes (so that killing group is possible in case of failure)
	int grouping_res = setpgid(0, 0);
	if (grouping_res < 0)
	{
		fprintf(stderr, "Process grouping failed\n");
		goto error_exit;
	}

	//fork dispatcher
	pid_t pid = fork();
	if (p < 0)
	{
		fprintf(stderr, "Dispatcher fork failed\n");
		goto fork_error_exit;
	}
	if (p == 0) //true for child (dispatcher)
	{
		dispatcher_process(shm, sems, file, args.O);
	}

	//fork carts
	for (int v_id = 1; v_id <= args.V; v_id++)
	{
		pid = fork();
		if (pid < 0)
		{
			fprintf(stderr, "Cart (V%d) fork failed\n", v_id);
			goto fork_error_exit;
		}
		if (pid == 0) //true for child (cart)
			cart_process(shm, sems, file, v_id, args.TV, args.K);
	}

	//fork visitors
	for (int n_id = 1; n_id < args.N; n_id++)
	{
		pid = fork();
		if (pid < 0)
		{
			fprintf(stderr, "Visitor (N%d) fork failed\n", n_id);
			goto fork_error_exit;
		}
		if (pid == 0) //true for child (visitor)
			visitor_process(shm, sems, file, n_id, TN);
	}

	//wait until all child processes end
	while (wait(NULL) > 0);

	//free shared memory and semaphores, close file
	semaphores_dtor(sems);
	shared_dtor(shm);
	fclose(file);

	return 0;

  fork_error_exit:
  	//send termination signal
  	kill(0, SIGTERM);

	//wait for all children to end (until returned PID is -1)
	while (wait(NULL) > 0);
  error_exit:
	//free shared memory and semaphores, and close file
	semaphores_dtor(sems);
	shared_dtor(shm);
	fclose(file);

	exit(1);
}


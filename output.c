#include "output.h"

void print_action(shared_t *shm, sem_t *mutex, FILE *file, const char *fmt, ...)
{
	//read arguments to argument list
	va_list fmt_args;
	va_start(fmt_args, fmt);

	sem_wait(mutex);
	/* inside mutex */

	//print number of action
	fprintf(file, "%d: ", shm->action_counter);
	shm->action_counter++;

	//print what the caller has to say
	vfprintf(file, fmt, fmt_args);
	fflush(file);

	/* exitting mutex */
	sem_post(mutex);

	va_end(fmt_args);
}


#include "output.h"

void print_action(shared *shm, sem_t mutex, FILE *file, const char *fmt, ...)
{
	//read arguments to argument list
	va_list fmt_args;
	va_start(fmt_args, fmt);

	sem_wait(mutex);
	/* inside mutex */

	//print number of action
	fprintf(file, "%d: ", shm->action_counter);
	action_counter++;

	//print what the caller has to say
	vfrpintf(file, fmt, fmt_args);
	fflush(f);

	/* exitting mutex */
	sem_post(mutex);

	va_end(fmt_args);
}


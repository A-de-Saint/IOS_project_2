#include "output.h"

void print_action(shared_t *shm, FILE *file, const char *fmt, ...)
{
	//read arguments to argument list
	va_list fmt_args;
	va_start(fmt_args, fmt);

	//print number of action
	fprintf(file, "%d: ", shm->action_counter);
	shm->action_counter++;

	//print what the caller has to say
	vfprintf(file, fmt, fmt_args);
	fflush(file);

	va_end(fmt_args);
}


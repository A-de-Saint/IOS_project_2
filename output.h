#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include <stdarg.h>
#include <semaphore.h>
#include "shared_mem.h"

//prints action to file synchronously
//mutex MUST BE LOCKED when calling this function
void print_action(shared_t *shm, FILE *file, const char *fmt, ...);

#endif


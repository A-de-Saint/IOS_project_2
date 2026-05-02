#ifndef PROCESSES_H
#define PROCESSES_H

#include "semaphores.h"
#include "shared_mem.h"
#include "output.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void dispatcher_process(shared_t *shm, semaphores_t *sem, FILE *file, int O);

void cart_process(shared_t *shm, semaphores_t *sem, FILE *file, int v_id, int TV, int cart_capacity);

void visitor_process(shared_t *shm, semaphores_t *sem, FILE *file, int n_id, int TN);

#endif

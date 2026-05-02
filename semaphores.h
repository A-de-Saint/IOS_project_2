/**
 * @file semaphores.h
 * @author Antonín Hrnčíř <xhrncia00>
 * @brief Header for semaphores
 * @date 2026-05-02
 */

#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include <semaphore.h>
#include <sys/mman.h>

//structure containing all the needed semaphores
typedef struct {
	sem_t mutex; 			//semaphore for shared memory and file manipulation	(GREEN)
	sem_t cart_in; 			//signal that next cart may start boarding			(RED)
	sem_t cart_boarding; 	//signal that visitors may board					(RED)
	sem_t cart_full; 		//signal that cart is full							(RED)
	sem_t cart_leaving; 	//signal that cart is departing						(RED)
	sem_t cart_emptying; 	//signal that cart is in its emptying phase			(GREEN)
	sem_t leave_cart; 		//signal that visitors should leave the cart		(RED)
	sem_t cart_emptied; 	//signal that cart is empty (after everyone left)	(RED)
	sem_t all_finished;		//signal that every forked process ended			(RED)
} semaphores_t;

//initializes all semaphores of sems
//if failed, frees all semaphores allocated up to that point and returns false
semaphores_t *semaphores_init();

void semaphores_dtor(semaphores_t *sems);

#endif


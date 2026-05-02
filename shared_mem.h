/**
 * @file shared_mem.h
 * @author Antonín Hrnčíř <xhrncia00>
 * @brief Header for shared memory
 * @date 2026-05-02
 */

#ifndef SHARED_MEM_H
#define SHARED_MEM_H

#include <sys/mman.h>
#include <stdbool.h>

//struct of shared info
typedef struct {
	int action_counter; 	//counts number of actions made 							(1)
	int visitors_left; 		//number of visitors left to escort 						(visitors_N)
	int to_be_boarded; 		//number of visitors that fit into the current cart 		(undef)
	int boarding_cart_id; 	//the id of the cart that is currently open for boarding 	(undef)
	int to_leave_cart; 		//number of visitors left to leave the cart 				(undef)
	int cart_departure_num;	//number for a cart to claim when departing					(0)
	int cart_arrival_num;	//a cart with this number is allowed to start leaving		(0)
	int leaving_cart_id; 	//the id of the cart that is currently open for leaving 	(undef)
	bool closing; 			//whether the attraction is closing or not 					(false)
} shared_t;

//maps and initializes shared memory (according to info in '()' above)
shared_t *shared_mem_init(int visitors_N);

//unmaps shared memory
void shared_dtor(shared_t *shm);

#endif


/**
 * @file args_parser.h
 * @author Antonín Hrnčíř <xhrncia00>
 * @brief Header for the argument parser
 * @date 2026-05-02
 */

#ifndef ARGS_PARSER_H
#define ARGS_PARSER_H

#include <stdbool.h>

//expected argument count
#define EXPECTED_ARG_COUNT 6

//constraints for all agruments
#define V_MIN 1
#define V_MAX 9

#define N_MIN 1
#define N_MAX 9999

#define K_MIN 4
#define K_MAX 40

#define TV_MIN 0
#define TV_MAX 1000

#define TN_MIN 0
#define TN_MAX 1000

#define O_MIN 1
#define O_MAX 100

//structure of arguments
typedef struct {
	int V; //number of carts
	int N; //number of visitors
	int K; //cart capacity
	int TV; //duration of a ride (microseconds)
	int TN; //max time before a visitor enters the queue (microseconds)
	int O; //minimal time offset of carts (microseconds)
} args_t;

//parses argc arguments from argv, saves to args
//does NOT check for NULL
//argv must contain ONLY the arguments that are to be read
//returns true if successful
bool parse_args(int argc, char *argv[argc], args_t *args);

#endif


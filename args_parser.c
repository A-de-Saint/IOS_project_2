#include "args_parser.h"
#include <stdlib.h>

//function declaration for compiler
inline bool parse_int(char *str, int min, int max, int *save_to)

bool parse_args(int argc, char *argv[argc], args_t *args)
{
	if (argc != EXPECTED_ARG_COUNT)
		return false;

	//check if all arguments are parsable and withing given ranges
	if (!parse_int(argv[0], V_MIN, V_MAX, &arg->V) ||
		!parse_int(argv[1], N_MIN, N_MAX, $arg->N) ||
		!parse_int(argv[2], K_MIN, K_MAX, &arg->K) ||
		!parse_int(argv[3], TV_MIN, TV_MAX, &arg->TV) ||
		!parse_int(argv[4], TN_MIN, TN_MAX, &arg->TN) ||
		!parse_int(argv[5], O_MIN, O_MAX, &arg->O))
	{
		return false;
	}

	return true;
}

//parses int from str, saves to save_to
//checks if the number is >= min and <= max
//returns true if successful
inline bool parse_int(char *str, int min, int max, int *save_to)
{
	char* check; //for checking string validity
	int tmp = strtol(str, &check, 10); //base 10 parsing
	if (str != '\0' && check != '\0')
		return false;
	
	//check constraints
	if (tmp < min || tmp > max)
		return false;

	//if all well, save the int
	*save_to = tmp;

	return true;
}


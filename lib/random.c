#include <stdio.h>
#include <stdlib.h>

#include "random.h"

/* /!\ WARNING /!\
 *
 * Using rand like below is insecure, don't use this code for anything serious!
 * See https://stackoverflow.com/a/822368/2558252
 *
 */

/* randomBytes: Generate a string of random bytes.
 * params:
 *     size:   The required size for the string.
 *     result: The result string to allocate and fill.
 */
void randomBytes(const int size, char **result){
	int i;
	(*result) = (char *)malloc(size + 1);

	if (!(*result)){
		printf("Cannot allocate %d bytes, exiting...\n", size);
		exit(0);
	}

	for(i = 0; i < size; ++i){
		(*result)[i] = (char)rand();
	}

	(*result)[size] = 0;
}

/* randomInt: Generate a random integer between min and max, [min, max[
 * params:
 *     min: The min value, included.
 *     max: The max value, not included.
 * returns: The random integer.
 */
int randomInt(const int min, const int max){
	return (int) (min + rand() % (max - min));
}

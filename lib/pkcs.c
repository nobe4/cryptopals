#include <stdlib.h>
#include <string.h>

#include "pkcs.h"

/* pkcs7Padding: Perform a PKCS7 padding on a string.
 * params:
 *     original:      The original string to pad.
 *     required_size: The new size of the string, after padding.
 *     result:        The string to fill with the data.
 * returns: The number of missing bytes.
 */
int pkcs7Padding(const char *original, const int required_size, char **result){

	int org_length = strlen(original);
	int length_diff = required_size - org_length;
	int i;

	// If the original string is too long, return the length diff.
	if (length_diff < 0){
		return length_diff;
	}
	
	// Allocate the new string
	(*result) = (char*)malloc(required_size + 1);

	// Copy the original string
	for(i = 0; i < org_length; i++){
		(*result)[i] = original[i];
	}

	// Add the padding
	for(; i < required_size; i++){
		(*result)[i] = length_diff;
	}

	// Properly end the string
	(*result)[required_size] = 0;

	return length_diff;
}

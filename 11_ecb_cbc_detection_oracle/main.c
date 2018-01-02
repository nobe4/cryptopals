#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../lib/misc.h"
#include "../lib/random.h"
#include "../lib/oracle.h"

int main(void){
	// Init the PRGN
	srand(time(NULL));

	char *input = NULL,
			 *partial_input = NULL,
			 *encrypted = NULL;

	int bytes_block = 16,
			encrypted_len,
			block_length,
			index = 0,
			current_repetition_count = 0,
			best_repetition_count = 0;

	// Generate a string made of 10 blocks of 16 bytes.
	randomBytes(bytes_block, &partial_input);
	repeatString(partial_input, bytes_block, 10, &input);

	encrypted_len = oracleEncrypt(input, strlen(input), &encrypted);
	block_length = encrypted_len / 2;

	// Doing a repetition search, starting at a block size of 2 and ending at a
	// block size of encrypted_len/4. It's going to be really fast to search for
	// the big blocks repetitions, so start from the high end.
	for(; block_length --> 2;) {

		// Because the padding is unknown we will test for repeating characters,
		// and count the concurrent ones. e.g. abcdecde will count to 3.
		for(index = 0; index < encrypted_len - block_length; index ++){

			if(encrypted[index] == encrypted[index+block_length]){
				current_repetition_count++;
			} else {
				if (current_repetition_count >  best_repetition_count){
					best_repetition_count = current_repetition_count;
				}
				current_repetition_count = 0;
			}
		}
	}

	// If the best found is equal or superior to the block size, we found a
	// repeation pattern, it it's then ECB mode.
	printf("A repetition of size %d was found, the mode is probably %s.\n",
			best_repetition_count, (best_repetition_count >= bytes_block)?"ECB":"CBC");

	return 0;
}

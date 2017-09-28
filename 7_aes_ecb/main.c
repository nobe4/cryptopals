#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../lib/file.h"
#include "../lib/base64.h"
#include "../lib/misc.h"
#include "../lib/aes.h"

int main(void){
	char *input = NULL,     // Input text from the file
			 *key = "YELLOW SUBMARINE",
			 *ascii = NULL,
			 *output = NULL;

	// Read the file
	readFile("input.txt", &input);

	// Remove the newlines from the input
	removeCharFromString(input, '\n');

	// Decode the base64 string
  int input_length = base64Decode(input, &ascii);

	// Decrypt the string
	AES_ECB_decrypt(ascii, input_length, key, &output);

	printf("%s\n", output);

	if (input) free(input);
	if (ascii) free(ascii);
	if (output) free(output);

	return 0;
}

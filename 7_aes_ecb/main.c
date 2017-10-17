#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../lib/file.h"
#include "../lib/base64.h"
#include "../lib/misc.h"
#include "../lib/hex.h"
#include "../lib/aes.h"

/* testDecryptNist: Run a decryption against a known result computed by NIST.
 * From http://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38a.pdf
 */
void testDecryptNist(){
	// F.1.2 ECB AES 128 Decrypt, page 24
	// Escape the key and input, but not the expected_output, it will be easier
	// to display.
	char *key = "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c",
			 *input = "\x3a\xd7\x7b\xb4\x0d\x7a\x36\x60\xa8\x9e\xca\xf3\x24\x66\xef\x97",
			 *expected_output = "6bc1bee22e409f96e93d7e117393172a",
			 *output = NULL,
			 *output_ascii = NULL;

	// Fixed and known size
	int input_length = 16;

	// Decrypt the string
	AES_ECB_decrypt(input, input_length, key, &output);
	ASCIIToHex(output, &output_ascii);

	// Cut the result to the correct size.
	output_ascii[32] = 0;

	printf("%s\n", output_ascii);
	printf("%s\n", expected_output);

	if (output) free(output);
	if (output) free(output_ascii);
}

/* testEncryptNist: Run an encryption against a known result computed by NIST.
 * From http://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38a.pdf
 */
void testEncryptNist(){
	// F.1.2 ECB AES 128 Decrypt, page 24
	// Escape the key and input, but not the expected_output, it will be easier
	// to display.
	char *key = "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c",
			 *input = "\x6b\xc1\xbe\xe2\x2e\x40\x9f\x96\xe9\x3d\x7e\x11\x73\x93\x17\x2a",
			 *expected_output = "3ad77bb40d7a3660a89ecaf32466ef97",
			 *output = NULL,
			 *output_ascii = NULL;

	// Fixed and known size
	int input_length = 16;

	// Decrypt the string
	AES_ECB_encrypt(input, input_length, key, &output);
	ASCIIToHex(output, &output_ascii);

	// Cut the result to the correct size.
	output_ascii[32] = 0;

	printf("%s\n", output_ascii);
	printf("%s\n", expected_output);

	if (output) free(output);
	if (output) free(output_ascii);
}

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

	// Extra tests for know values
	/* testDecryptNist(); */
	/* testEncryptNist(); */

	return 0;
}

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
	// F.2.1 CBC AES 128 Decrypt, page 27
	// Escape the key and input, but not the expected_output, it will be easier
	// to display.
	char *key = "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c",
			 *iv = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
			 *input = "\x76\x49\xab\xac\x81\x19\xb2\x46\xce\xe9\x8e\x9b\x12\xe9\x19\x7d",
			 *expected_output = "6bc1bee22e409f96e93d7e117393172a",
			 *output = NULL,
			 *output_ascii = NULL;

	// Fixed and known size
	int input_length = 16;

	// Decrypt the string
	AES_CBC_decrypt(input, input_length, key, iv,  &output);
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
	// F.2.1 CBC AES 128 Encrypt, page 27
	// Escape the key and input, but not the expected_output, it will be easier
	// to display.
	char *key = "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c",
			 *iv = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
			 *input = "\x6b\xc1\xbe\xe2\x2e\x40\x9f\x96\xe9\x3d\x7e\x11\x73\x93\x17\x2a",
			 *expected_output = "7649abac8119b246cee98e9b12e9197d",
			 *output = NULL,
			 *output_ascii = NULL;

	// Fixed and known size
	int input_length = 16;

	// Decrypt the string
	AES_CBC_encrypt(input, input_length, key, iv,  &output);
	ASCIIToHex(output, &output_ascii);

	// Cut the result to the correct size.
	output_ascii[32] = 0;

	printf("%s\n", output_ascii);
	printf("%s\n", expected_output);

	if (output) free(output);
	if (output) free(output_ascii);
}

int main(void){
	char *input = NULL,
			 *key = "YELLOW SUBMARINE",
			 *iv = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
			 *ascii = NULL,
			 *output = NULL;

	// Read the file
	readFile("input.txt", &input);

	// Remove the newlines from the input
	removeCharFromString(input, '\n');

	// Decode the base64 string
	int input_length = base64Decode(input, &ascii);

	// Decrypt the string
	AES_CBC_decrypt(ascii, input_length, key, iv, &output);

	printf("%s\n", output);

	if (input) free(input);
	if (ascii) free(ascii);
	if (output) free(output);

	// Extra tests for know values
	/* testDecryptNist(); */
	/* testEncryptNist(); */

	return 0;
}

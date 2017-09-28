#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../lib/hex.h"
#include "../lib/base64.h"

void testDecode(char *input, char* expected_output);
void testEncode(char *input, char* expected_output);

int main(){
	char input[] = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
	char expected_output[] = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";

	// Convert to ASCII
	char *input_string = NULL;
	hexToASCII(input, &input_string);

	printf("[D] Hex to string:\n %s\n %s\n\n", input, input_string);

	testEncode(input_string, expected_output);

	testDecode("aGVsbG8=", "hello");
	testDecode("aGVsbG8gMQ==", "hello 1");
	testDecode("aGVsbG8gMTA=", "hello 10");
	testDecode("aGVsbG8gMTAw", "hello 100");
	testDecode("YWFhYWFhYWFhYWFhYWFhYWFhYWFhYWFhYWFhYWFhYWFhYWFhYWFhYWFhYWFhYWFhYWE=", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

	return 0;
}

void testDecode(char *input, char *expected_output){
	char *output = NULL;
	base64Decode(input, &output);

	printf("[D] String from base64:\n %s\n => %s\n\n", input, output);

	char valid = strcmp(output, expected_output);
	printf("[V] '%s' ?= '%s' : \n\t%s\n\n", output, expected_output, valid?"False":"True");

	if(output) free(output);
}

void testEncode(char *input, char *expected_output){
	char *output = NULL;
	base64Encode(input, &output);

	printf("[D] String from base64:\n %s\n => %s\n\n", input, output);

	char valid = strcmp(output, expected_output);
	printf("[V] '%s' ?= '%s' : \n\t%s\n\n", output, expected_output, valid?"False":"True");

	if(output) free(output);
}

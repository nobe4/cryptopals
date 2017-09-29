#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../lib/pkcs.h"

int main(int argc, char *argv[]){

	char *string = "YELLOW SUBMARINE";
	char *padded_string = NULL;
	int required_size = 20;
	int i;

	printf("Missing bytes: %d, new size: %d\n", pkcs7Padding(string, required_size, &padded_string), (int)strlen(padded_string));
	for (i = 0; i < required_size; i ++){
		printf("%02x ", padded_string[i]);
	}
	printf("\n");

	if (padded_string) free(padded_string);

	return 0;
}

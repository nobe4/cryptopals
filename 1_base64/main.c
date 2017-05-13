#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../lib/hex.h"
#include "../lib/base64.h"

int main(){
  char input[] = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
  char expected_output[] = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";

  // Convert to ASCII
  char *input_string = NULL;
  char string_len = hexToASCII(input, &input_string);

  printf("[D] Hex to string:\n %s\n %s\n\n", input, input_string);

  // Convert to base64
  char *output = NULL;
  int length = base64Encode(input_string, &output);

  printf("[D] String to base64:\n %s\n %s\n\n", input_string, output);

  char valid = strcmp(output, expected_output);
  printf("[V] %s ?= %s: \n\t%s\n\n", output, expected_output, valid?"False":"True");

  if(output) free(output);
  if(input_string) free(input_string);

  if (valid == 0) return 0;
  return 1;
}

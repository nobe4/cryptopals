#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../lib/hex.h"
#include "../lib/xor.h"

int main(void){

  char *input = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal",
       *xored = NULL,
       *hex = NULL,
       *key = "ICE",
       *expected_output = "0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f";

  int input_len = strlen(input),
      key_len = strlen(key),
      valid = 0;

  xor(input, input_len, key, key_len, &xored);
  ASCIIToHex(xored, &hex);

  valid = strcmp(hex, expected_output);

  printf("[V] %s ?= %s\n\t%s\n\n", hex, expected_output, valid?"False":"True");

  if(xored) free(xored);

  if(hex) free(hex);

  return 0;
}

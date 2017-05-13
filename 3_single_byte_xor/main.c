#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../lib/hex.h"
#include "../lib/xor.h"
#include "../lib/misc.h"

int main(void){

  char *input = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736",
       *ASCII_input,
       xor_key[1],
       *output;

  int ASCII_len; // Length of the ASCII string

  ASCII_len = hexToASCII(input, &ASCII_input);

  // XOR with the winning key
  xor_key[0] = detectSingleByteXORKey(ASCII_input, ASCII_len, englishScore);

  xor(ASCII_input, ASCII_len, xor_key, 1, &output);
  printf("[V] Found string, should be plain english:\n%s \n\n", output);

  return 0;
}

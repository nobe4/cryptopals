#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/hex.h"
#include "../lib/xor.h"

int main(){

  char* string1_hex = "1c0111001f010100061a024b53535009181c";
  char* string1_ascii;
  int str1_len;

  char* string2_hex = "686974207468652062756c6c277320657965";
  char* string2_ascii;
  int str2_len;

  char* output;
  char* xored_string;
  int xor_success;
  char valid = -1;

  char* expected_output = "746865206b696420646f6e277420706c6179";

  // Convert both strings from hex to ASCII and keep the sizes around
  str1_len = hexToASCII(string1_hex, &string1_ascii);
  str2_len = hexToASCII(string2_hex, &string2_ascii);

  printf("[D] Hex strings to ASCII lenght: %d %d\n", str1_len, str2_len);

  // Call the fixed XOR function, will store the result in xored_string
  xor_success = xor(string1_ascii, str1_len, string2_ascii, str2_len, &xored_string);

  printf("[D] XOR success: %d\n", xor_success);

  if(xor_success > -1){
    // Convert the xored string back to hex
    ASCIIToHex(xored_string, &output);

    valid = strcmp(output, expected_output);
    printf("[V] %s ?= %s\n\t%s\n\n", output, expected_output, valid?"False":"True");

  }

  if (string1_ascii) free(string1_ascii);
  if (string2_ascii) free(string2_ascii);
  if (output) free(output);
  if (xored_string) free(xored_string);

  if (valid == 0){
    return 0;
  }

  return 1;
}

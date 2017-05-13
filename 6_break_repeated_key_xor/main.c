#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../lib/xor.h"
#include "../lib/misc.h"
#include "../lib/file.h"

int main(void){
  char *input = NULL,     // Input text from the file
       **blocks = NULL,   // Transposed blocks
       *output = NULL,    // Un XORed text
       *xor_key = NULL;   // XOR key

  int input_length = 0,   // Length of the file
      key_len = 0,        // Found XOR key length
      block_number = 0,   // Number of blocks
      block_len = 0,      // Length of the blocks
      i;                  // Temporary variable

  // Read the file
  input_length = readFile("input.txt", &input);

  // Find the key length
  key_len = detectXORKeyLen(input, input_length, 2, 40);
  printf("[D] Possible key length found: %d\n", key_len);

  // Allocate the XOR key
  xor_key = (char*)malloc(key_len);

  if (!xor_key){
    printf("[E] Could not allocate %d bytes for the xor key.\n", key_len);
    exit(1);
  }

  // Transpose the string by blocks
  block_len = transposeByBlocks(input, input_length, key_len, &blocks);
  printf("[D] Transposed into %d blocks of %d bytes.\n", key_len, block_len);


  printf("[D] Detecting the xor key: ");
  for(i = 0; i < key_len; i ++){
    xor_key[i] = detectSingleByteXORKey(blocks[i], key_len, englishScore);
    printf("%c", xor_key[i]);
  }
  // End the key
  xor_key[i] = 0;

  // XOR the text with the found key
  xor(input, input_length, xor_key, key_len, &output);

  // The text is really long. To display it, end after a few lines with a '...'
  strncpy(output + 100, "...\x00", 4);

  printf("\n[V] Found text:\n%s\n\n", output);

  // Free everything
  if (blocks && block_number) {
    for(i = 0; i < block_number; i++) {
      if(blocks[i]) free(blocks[i]);
    }
    free(blocks);
  }
  if (input) free(input);
  if (output) free(output);
  if (xor_key) free(xor_key);

  return 0;
}

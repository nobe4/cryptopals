#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../lib/hex.h"
#include "../lib/xor.h"
#include "../lib/misc.h"

// Each line is 60 chars + '\x00'
#define LINE_LENGTH 61

int main(void){

  char *ASCII_input = NULL, // current ASCII converted line
       xor_key[1],          // Current xor key found
       *xored_line = NULL,  // Current xored line
       line[LINE_LENGTH],   // Current line
       *best_line = NULL;   // Best found line

  int ASCII_len = 0,        // Length of the ASCII string
      best_score = 0,       // Best found score
      current_score = 0;    // Current found score

  FILE* file = fopen("4.txt", "r"); // Input file

  if (!file){
    printf("[E] Can't open file.");
    return 1;
  }

  while (
      // Go through all lines in the file
      fgets(line, LINE_LENGTH, file)
      ) {

    ASCII_len = hexToASCII(line, &ASCII_input);

    // Detect the best XOR key for this string
    xor_key[0] = detectSingleByteXORKey(ASCII_input, ASCII_len, englishScore);
    // XOR the string with the found char
    xor(ASCII_input, ASCII_len, xor_key, 1, &xored_line);

    // Get the english score for the current string
    current_score = englishScore(xored_line);

    // Keep the best score possible
    if (current_score > best_score){
      best_score = current_score;

      // If the best line is not allocated, do it
      if (!best_line){
        best_line = (char*)malloc(ASCII_len);
      }

      // Copy the xored_line in the best line, for later use.
      strcpy(best_line, xored_line);
    }

  }

  printf("[V] Found string: %s \n\n", best_line);

  fclose(file);

  if(ASCII_input) free(ASCII_input);
  if(best_line) free(best_line);
  if(xored_line) free(xored_line);

  return 0;
}

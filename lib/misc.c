#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "misc.h"

/* englishScore: Perform a simple string character frenquency analysis.
 * params:
 *    string: The string to analyse
 * returns: The string score
 */
int englishScore(const char* string) {
  // Source: https://en.wikipedia.org/wiki/Letter_frequency#Relative_frequencies_of_letters_in_the_English_language
  int frequencies[] = {
    8167,   // a
    1492,   // b
    2782,   // c
    4253,   // d
    12702,  // e
    2228,   // f
    2015,   // g
    6094,   // h
    6966,   // i
    153,    // j
    772,    // k
    4025,   // l
    2406,   // m
    6749,   // n
    7507,   // o
    1929,   // p
    95,     // q
    5987,   // r
    6327,   // s
    9056,   // t
    2758,   // u
    978,    // v
    2360,   // w
    150,    // x
    1974,   // y
    74,     // z
  };

  int character_value, // current charater value out of the string
      i,
      score = 0;

  for(i=strlen(string) - 1; i >= 0; i --){

    character_value = (int)string[i];

    // Space character
    if( character_value == 32 ){
      score += 10000;
    } else
    // Uppercase
    if( character_value >= 65 && character_value <= 90){
      score += frequencies[character_value - 65];
    } else
    // Lowercase
    if( character_value >= 97 && character_value <= 122){
      score += frequencies[character_value - 97];
    }
    // Move down the non-letters
    else {
      score -= 1000;
    }
  }

  return score;
}

/* hammingDistance: Compute the distance between two strings
 * params:
 *    string1:  The first string
 *    str1_len: The length of the first string
 *    string2:  The second string
 *    str2_len: The length of the first string
 * returns: 
 *  The bits difference between the two strings
 *  -1 if there was an error
 */
int hammingDistance(const char* string1, const int str1_len, const char* string2, const int str2_len){
  // Make sure the strings are the same size
  if(str1_len != str2_len) return -1;

  int score = 0,
      current_xor = 0,
      i;

  for(i = 0; i < str1_len; i ++){
    // Get current xored byte between strings
    current_xor = string1[i] ^ string2[i];

    // Naïve set bit counting
    // Try to understand http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
    // http://stackoverflow.com/questions/14555607/explanation-required-number-of-bits-set-in-a-number
    do {
      score += current_xor & 1;
    } while(current_xor >>= 1);

  }

  return score;
}

/* transposeByBlocks: Transform a string into an array of strings, transposed.
 * e.g. abcdef, for two blocks will be: [ace, bdf], for 3 [ad, be, cf]
 * params:
 *    string:   The string to transpose
 *    str_len:  The string length
 *    blocks:   The resulting blocks
 * returns: The lenght of each block
 */
int transposeByBlocks(const char* string, const int str_len, const int block_number, char ***blocks){
  int block_len, i, j;

  // Compute the length of a block, will be rounded down due to integer
  // division. That's fine, it's better to end 1 block before than to have a
  // block half-filled.
  block_len = str_len / block_number;

  // Allocate the full block
  (*blocks) = (char **)malloc(sizeof(char *) * block_number);

  if(!(*blocks)){
    printf("[E] Could not allocate %d blocks, aborting...\n", block_number);
    exit(1);
  }

  for(i = 0; i < block_number; i++){
    // Allocate the block
    (*blocks)[i] = (char *)malloc(block_len);

    if(!(*blocks)[i]){
      printf("[E] Could not allocate the block number %d, aborting...\n", i);
      exit(1);
    }

    // Fill the newly created block
    for(j = 0; j < block_len; j ++){
      (*blocks)[i][j] = string[ i + j * block_number ];
    }
  }

  return block_len;
}


/* removeCharFromString: Remove a defined char from a string
 * Inspired by https://stackoverflow.com/a/9895318/2558252
 * params:
 * 		string:  the string to remove from
 * 		theChar: the char to remove from the string
 */
void removeCharFromString(char* string, char theChar){
	char *read_ptr = string,
			 *write_ptr = string;

	// Read the whole string
	while(*read_ptr){
		// Write the current char and increment the read pointer
		*write_ptr = *read_ptr++;

		// Only increment the write pointer if the current char is not theChar
		write_ptr += (*write_ptr != theChar);
	}

	// Nicely end the string
	*write_ptr = 0;
}

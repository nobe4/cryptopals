#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "xor.h"
#include "misc.h"


/* xor: Perform a xor between two fixed-size strings.
 * The resulting string will have the size of the first string, the second
 * string will serve as a repeating key.
 * params:
 *  string1:   First string to xor, reference string
 *  str1_len:  First string length
 *  string2:   Second string to xor, key string
 *  str2_len:  Second string length
 *  result:    Result buffer
 */
int xor(const char* string1, const int str1_len, const char* string2, const int str2_len, char** result){
  int i = 0;

  // Allocate the new string
  (*result) = (char*)malloc(str1_len);

  // If the allocation failed, break and return -1
  if (!(*result)) return -1;

  for(i = 0; i < str1_len; i ++){
    (*result)[i] = string1[i] ^ string2[i % str2_len];
  }

  return 0;
}

/* detectSingleByteXORKey: Try multiple XOR key and find the best solution based on the score function.
 * params:
 *    original:       The original string to be tested
 *    org_len:        The original string length
 *    score_function: The scoring function to use
 * return:
 *    The char that is the best XOR key
 */
char detectSingleByteXORKey(const char* original, const int org_len, int(*score_function)(const char*)){
  char *buffer;

  int best_score = 0,   // Best english score
      best_xor_key = 0, // Corresponding XOR key to the best english score
      current_score,    // Temporary score
      i;

    char current_key[1];   // Current tested xor key

  // Try all ASCII XOR key
  for(i = 0; i < 255; i ++){
    current_key[0] = (char)i;

    // Compute the XORed string
    xor(original, org_len, current_key, 1, &buffer);

    // Compute the score
    current_score = score_function(buffer);

    // Update the best score and key
    if (current_score > best_score) {
      best_xor_key = i;
      best_score = current_score;
    }

  }

  return (char)best_xor_key;
}

/* detectXORKeyLen: Compute hammings distance to find a possible XOR key lenght
 * params:
 *    string:           The string to search the key into
 *    str_len:          The string lenght
 *    min_key_length:   The minimum XOR key to search for
 *    max_key_length:   The maximum XOR key to search for
 * returns: The possible length key found
 */
int detectXORKeyLen(const char* string, const int str_len, const int min_key_length, const int max_key_len){
  int current_key_len = min_key_length, // Start at the minimal key length
      current_distance_len = 0,         // Current length distance being tested
      best_key_len = 0,                 // Best key length found
      current_max_distance_num,         // How much distance we can compute ?
      best_distance = INT_MAX;          // The best distance is the minimal value possible, so start at the max

  float current_distance = 0.0;         // Current calculated distance

  // For each possible key length ...
  for(; current_key_len <= max_key_len; current_key_len ++){
    // Reset the current distance
    current_distance = 0.0;

    // Maximum number fo difference to compute
    current_max_distance_num = (str_len/current_key_len) - 1;

    // For each n & n+1 combination of strings, compute the hamming distance
    for(current_distance_len = 0; current_distance_len < current_max_distance_num; current_distance_len++){

      // Add the normalized difference
      current_distance += hammingDistance(
          string + (current_key_len * current_distance_len), current_key_len,
          string + (current_key_len * (current_distance_len+1)) , current_key_len
          ) / (float)current_key_len;
    }

    // Normalize to the number of tested distances
    current_distance /= (float)current_max_distance_num;

    // Update the best distances and key length
    if(current_distance < best_distance){
      best_distance = current_distance;
      best_key_len = current_key_len;
    }
  }

  return best_key_len;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hex.h"

/* hexToASCII: Convert a string from hex to its ASCII representation:
 *    e.g.:  deadbeef will be converted to \xde\xad\xbe\xef, or the ASCII equivalent
 * params:
 *    original: Original string, passed directly
 *    result:   Result string, passed as pointer
 * return: Size of the result string.
 */
int hexToASCII(const char* original, char **result){
  // The result string will be half the size of the original as the hex is
  // coded in two bytes.
  int org_len = strlen(original),   // Original string length
      rst_len = org_len / 2,        // Result string length
      i;                            // Generic temporary variable

  // Current part of the original string we are considering, the size is 3 to
  // have the '\x00' byte terminating the string.
  char org_part[3];
  org_part[2] = 0;

  (*result) = (char *)malloc(rst_len + 1);

  // For each char in the result string
  for(i = 0; i < rst_len; i++){
    // Take the tow current chars of the original string
    org_part[0] = original[i*2];
    org_part[1] = original[i*2+1];
    // And convert them to a char, casted from a long.
    (*result)[i] = (char)strtol(org_part, NULL, 16);
  }

  // Terminate the string
  (*result)[i] = 0;

  return rst_len;
}

/* ASCIIToHex: Convert a string from ASCII to its hex representation:
 *    e.g.:  \xde\xad\xbe\xef will be converted to the hex equivalent deadbeef
 * params:
 *    original: Original string, passed directly
 *    result:   Result string, passed as pointer
 * return: Size of the result string.
 */
int ASCIIToHex(const char* original, char **result){
  // The result string will be half the size of the original as the hex is
  // coded in two bytes.
  int org_len = strlen(original),   // Original string length
      rst_len = org_len * 2,        // Result string length
      i;                            // Generic temporary variable

  (*result) = (char *)malloc(rst_len + 1);

  // For each char in the result string
  for(i = 0; i < org_len; i++){
    sprintf((*result)+i*2, "%02x", (unsigned char)(*(original+i)));
  }

  // Terminate the string
  (*result)[i*2] = 0;

  return rst_len;
}

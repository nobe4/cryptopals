#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* hexToString: Convert a string from hex to its string representation:
 *    e.g.:  deadbeef will be converted to \xde\xad\xbe\xef, or the ASCII equivalent
 * params:
 *    original: Original string, passed directly
 *    result:   Result string, passed as pointer
 * return: Size of the result string.
 */
int hexToString(const char* original, char **result){
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

/* base64Encode: Encode a string to a base64 string.
 * params:
 *    original: Original string, passed directly.
 *    result:   Result string, passed as pointer.
 * return: Size of the result string.
 */
int base64Encode(const char* original, char** result){
  // Base64 charset
  const char b64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  //         << 16    << 8
  // AAAAAAAA BBBBBBBB CCCCCCCC
  //       << 18  << 12  << 6   << 0
  // AAAAAA AABBBB BBBBCC CCCCCC

  int shifts[] = {18, 12, 6, 0};

  int b64_i,      // base 64 string index
      org_i = 0,  // original string index
      org_c,      // original string temorary char
      i;          // Generic temporary variable

  int org_len = strlen(original);

  // Compute the base64 length and pad length
  // ref: http://stackoverflow.com/a/32140193/2558252
  int b64_len = ((4 * org_len / 3) + 3) & ~3;
  int pad_size = 3 - (org_len % 3);

  // Reserve the resulting base64 string
  (*result) = (char *)malloc(b64_len + 1);

  for(b64_i = 0; b64_i < b64_len;){

    // Get the next 3 chars from the original string, shift and add them in the
    // original char, pad with 0.
    org_c =                      original[org_i++] << 16;
    org_c += (org_i > org_len)?0:original[org_i++] << 8;
    org_c += (org_i > org_len)?0:original[org_i++];

    // Convert to the b64 4 chars and add them to the result base 64 string.
    // This will write A even if there is no more data, that's fine, there is a
    // safe guard below to clean that.
    for(i = 0; i < 4; i ++){
      (*result)[b64_i++] = b64_chars[(org_c >> shifts[i]) & 63];
    }

  }

  // End the string with a NULL bit.
  (*result)[b64_i] = 0;

  // Add the necessary pad.
  if(pad_size != 3){
    for(b64_i = 1; b64_i <= pad_size; b64_i ++){
      (*result)[b64_len - b64_i]= '=';
    }
  }

  return b64_len;
}


int main(){
    char input[] = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    // output SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t

    // Convert to ASCII
    char *input_string = NULL;
    char string_len = hexToString(input, &input_string);

    printf("Hex to string:\n %s\n %s\n\n", input, input_string);

    // Convert to base64
    char *output = NULL;
    int length = base64Encode(input_string, &output);

    printf("String to base64:\n %s\n %s\n\n", input_string, output);

    if(output) free(output);
    if(input_string) free(input_string);

    return 0;
}

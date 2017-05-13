#include <stdlib.h>
#include <string.h>

#include "base64.h"
#include "hex.h"

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

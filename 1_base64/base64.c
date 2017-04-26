#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* base64encode: Encode a string to a base64 string.
 * params:
 *    original: Original string, passed directly
 *    result:   Result string, passed as pointer
 * return: Size of the result string.
 */
int base64encode( const char* original, char** result){
  // Base64 charset
  const char b64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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
    // This will write A evern if there is no more data, that's fine, there is
    // a safe guard below to clean that.
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
    char input[] = "\x49\x27\x6d\x20\x6b\x69\x6c\x6c\x69\x6e\x67\x20\x79\x6f\x75\x72\x20\x62\x72\x61\x69\x6e\x20\x6c\x69\x6b\x65\x20\x61\x20\x70\x6f\x69\x73\x6f\x6e\x6f\x75\x73\x20\x6d\x75\x73\x68\x72\x6f\x6f\x6d";
    // output SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t

    char *output = NULL;

    int length = base64encode(input, &output);

    printf("'%s' : %d %s\n", input, length, output);

    if(output) free(output);

    return 0;
}

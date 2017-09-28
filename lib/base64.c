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

/* base64Decode: Decode a base64 string to a plaintext string.
 * params: 
 *    original: Original string, passed directly.
 *    result: 	Result string, passed as a pointer.
 * return: size of the result string.
 * Greatly inspired by https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64#C_2
 */
int base64Decode(const char* original, char** result){
	//       << 18  << 12  << 6   << 0
	// AAAAAA AABBBB BBBBCC CCCCCC
	//         << 16    << 8
	// AAAAAAAA BBBBBBBB CCCCCCCC

	int rst_i = 0,	// base 64 string index
			org_i = 0,	// original string index
			org_c,			// original string temorary char
			t,					// temporary buffer
			i = 0;			// Generic temporary variable

	int org_len = strlen(original);

	// Find the padding size
	int pad_size = 0;
	while(original[org_len - pad_size - 1] == '='){
		pad_size ++;
	}

	// Compute the result string length and allocate the string
	int rst_len = (org_len * 3 / 4) - pad_size;
	(*result) = (char *)malloc(rst_len + 1);

	for(org_i = 0; org_i < org_len;){
		// Get the reversed chars in the table
		t = rev_chars[(int)original[org_i++]];

		// Invalid char, return an error
		if(t > 65) return -1;

		// Pad by 6 bits to create the A*6 B*6 C*6 D*6 int
		org_c = org_c << 6 | t;

		// Once we have the 4 original numbers, split into three
		if(++i == 4){
			(*result)[rst_i ++] = (org_c >> 16) & 255;
			(*result)[rst_i ++] = (org_c >> 8 ) & 255;
			(*result)[rst_i ++] = (org_c      ) & 255;

			// And reset the counters
			org_c = 0;
			i = 0;
		}
	}

	// If there are remaining chars, not in a 4-group, handle them here
	if (i == 3) {
		(*result)[rst_i++] = (org_c >> 10) & 255;
		(*result)[rst_i++] = (org_c >> 2) & 255;
	} else if (i == 2) {
		(*result)[rst_i++] = (org_c >> 4) & 255;
	}

	// End the string
	(*result)[rst_len] = 0;

	return rst_len;
}

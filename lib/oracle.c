#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "random.h"
#include "oracle.h"
#include "aes.h"
#include "misc.h"

#define CBC_MODE 1

int oracleEncrypt(const char *input, const int input_len, char **encrypted){
	char *prefix = NULL,
			 *suffix = NULL,
			 *input_extended = NULL,
			 *key = NULL,
			 *iv = NULL;

	int pre_len = randomInt(5, 11),
			suf_len = randomInt(5, 11),
			ext_len = input_len + pre_len + suf_len,
			aes_mode = randomInt(0, 2);

	randomBytes(pre_len, &prefix);
	randomBytes(suf_len, &suffix);

	surroundString(input, input_len, prefix, pre_len, suffix, suf_len, &input_extended);

	randomAESVector(&key);

	if (aes_mode == CBC_MODE){
		randomAESVector(&iv);
		AES_CBC_encrypt(input_extended, ext_len, key, iv, encrypted);
	} else {
		/* printf("ECB\n"); */
		AES_ECB_encrypt(input_extended, ext_len, key, encrypted);
	}
	
	free(prefix);
	free(suffix);
	free(input_extended);
	free(key);
	free(iv);

	return ext_len;
}


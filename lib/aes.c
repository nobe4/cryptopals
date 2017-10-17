#include <stdlib.h>
#include <string.h>

#include "aes.h"

// Constants for AES ECB 128
#define COLUMNS_NUMBER 4				// Number of columns
#define KEY_WORDS 4							// Number of 32 bit words in a key.
#define ROUNDS_NUMBER 10				// The number of rounds

// Define a typedef for smoother reading.
typedef char state_t[4][4];

/* keyExpansion: Produce the expanded key from the original AES key, will be used in all the
 * rounds of the AES flow.
 * From https://en.wikipedia.org/wiki/Rijndael_key_schedule
 * params:
 *     key:          The initial decryption key.
 *     expanded_key: The resulting expanded key to fill.
 */
static void keyExpansion(const char* key, char *expanded_key) {
	int i;
	unsigned char tmp[4], // Temporary buffer for the operations
								k;			// Temporary value for the rotation

	// The first round is just the original key.
	for (i = 0; i < KEY_WORDS; ++i) {
		expanded_key[(i * 4) + 0] = key[(i * 4) + 0];
		expanded_key[(i * 4) + 1] = key[(i * 4) + 1];
		expanded_key[(i * 4) + 2] = key[(i * 4) + 2];
		expanded_key[(i * 4) + 3] = key[(i * 4) + 3];
	}

	// The remaining rounds are combining the existing rounds with sbox and
	// operations.
	for (; i < COLUMNS_NUMBER * (ROUNDS_NUMBER + 1); ++i) {
		// Get the previous key
		tmp[0] = expanded_key[(i-1) * 4 + 0];
		tmp[1] = expanded_key[(i-1) * 4 + 1];
		tmp[2] = expanded_key[(i-1) * 4 + 2];
		tmp[3] = expanded_key[(i-1) * 4 + 3];

		// Computer here the new buffer used later, only once for each 4 bytes
		// combination.
		if (i % KEY_WORDS == 0) {
			// Shift to the left 1 byte: [0,1,2,3] => [1,2,3,0]
			k = tmp[0];
			tmp[0] = tmp[1];
			tmp[1] = tmp[2];
			tmp[2] = tmp[3];
			tmp[3] = k;

			// Apply the Rijndael S-box on all bytes of the buffer.
			tmp[0] = sbox[tmp[0]];
			tmp[1] = sbox[tmp[1]];
			tmp[2] = sbox[tmp[2]];
			tmp[3] = sbox[tmp[3]];

			// XOR the leftmost byte with 2^(i-1) in the Rijndael finite field
			// (values computed in Rcon).
			tmp[0] =  tmp[0] ^ Rcon[i/KEY_WORDS];
		}

		// The new 4 bytes are the previous ones, XORed with the buffer computed above.
		expanded_key[i * 4 + 0] = expanded_key[(i - KEY_WORDS) * 4 + 0] ^ tmp[0];
		expanded_key[i * 4 + 1] = expanded_key[(i - KEY_WORDS) * 4 + 1] ^ tmp[1];
		expanded_key[i * 4 + 2] = expanded_key[(i - KEY_WORDS) * 4 + 2] ^ tmp[2];
		expanded_key[i * 4 + 3] = expanded_key[(i - KEY_WORDS) * 4 + 3] ^ tmp[3];
	}
}

/* addRoundKey: Add the current round key into the state.
 * From https://en.wikipedia.org/wiki/Advanced_Encryption_Standard#The_AddRoundKey_step
 */
static void addRoundKey(state_t state, int round, const char* expanded_key) {
	// Loop through all 16 bytes of the state
	int i,j;
	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 4; ++j) {
			// ~= state[i][j] = key[round][i]
			state[i][j] ^= expanded_key[round * COLUMNS_NUMBER * 4 + i * COLUMNS_NUMBER + j];
		}
	}
}

/* multiply: Multiplication in a Gallois field, the multiplication is done
 * between two polynomial expressions, modulo 0x11b.
 * Look at https://en.wikipedia.org/wiki/Finite_field_arithmetic for an example
 * and the code comments.
 */
static uint8_t multiply(uint8_t x, uint8_t y) {
	uint8_t p = 0;

	while(x && y){
		if (y & 1)     p ^= x;

		if (x & 0x80)  x = (x << 1) ^ 0x11b;
		else           x <<= 1;
		
		y >>= 1;
	}

	return p;
}

/* mixColumns: Mix the columns, some heavy math here.
 * The logic is to construct the new state, one column (represented as
 * [a,b,c,d]) at a time.
 * From https://en.wikipedia.org/wiki/Rijndael_MixColumns
 */
static void mixColumns(state_t state) {
	int i;
	uint8_t a, b, c, d;
	for (i = 0; i < 4; ++i) {
		a = state[i][0];
		b = state[i][1];
		c = state[i][2];
		d = state[i][3];

		// In a Gallois Field, the addition is a XOR operation.
		// We're using the following factors:
		// a = a * 2 + b * 3 + c * 1 + d * 1
		// b = a * 1 + b * 2 + c * 3 + d * 1
		// c = a * 1 + b * 1 + c * 2 + d * 3
		// d = a * 3 + b * 1 + c * 1 + d * 2
		// From https://en.wikipedia.org/wiki/Rijndael_MixColumns
		state[i][0] = multiply(a, 0x02) ^ multiply(b, 0x03) ^ multiply(c, 0x01) ^ multiply(d, 0x01);
		state[i][1] = multiply(a, 0x01) ^ multiply(b, 0x02) ^ multiply(c, 0x03) ^ multiply(d, 0x01);
		state[i][2] = multiply(a, 0x01) ^ multiply(b, 0x01) ^ multiply(c, 0x02) ^ multiply(d, 0x03);
		state[i][3] = multiply(a, 0x03) ^ multiply(b, 0x01) ^ multiply(c, 0x01) ^ multiply(d, 0x02);
	}
}

/* invMixColumns: Inverse the column mixing, some heavy math here.
 * The logic is to construct the new state, one column (represented as
 * [a,b,c,d]) at a time.
 * From https://en.wikipedia.org/wiki/Rijndael_MixColumns
 */
static void invMixColumns(state_t state) {
	int i;
	uint8_t a, b, c, d;
	for (i = 0; i < 4; ++i) { 
		a = state[i][0];
		b = state[i][1];
		c = state[i][2];
		d = state[i][3];

		// In a Gallois Field, the addition is a XOR operation. Because we're doing
		// the inverse mix column, we're using the following factors:
		// a = a * 14 + b * 11 + c * 13 + d * 9
		// b = a * 9  + b * 14 + c * 11 + d * 13
		// c = a * 13 + b * 9  + c * 14 + d * 11
		// d = a * 11 + b * 13 + c * 9  + d * 14
		// From https://en.wikipedia.org/wiki/Rijndael_MixColumns#InverseMixColumns
		state[i][0] = multiply(a, 0x0e) ^ multiply(b, 0x0b) ^ multiply(c, 0x0d) ^ multiply(d, 0x09);
		state[i][1] = multiply(a, 0x09) ^ multiply(b, 0x0e) ^ multiply(c, 0x0b) ^ multiply(d, 0x0d);
		state[i][2] = multiply(a, 0x0d) ^ multiply(b, 0x09) ^ multiply(c, 0x0e) ^ multiply(d, 0x0b);
		state[i][3] = multiply(a, 0x0b) ^ multiply(b, 0x0d) ^ multiply(c, 0x09) ^ multiply(d, 0x0e);
	}
}

/* subBytes: Replace each byte in the state by its corresponding value in the S-box.
 * From https://en.wikipedia.org/wiki/Advanced_Encryption_Standard#The_SubBytes_step
 */
static void subBytes(state_t state) {
	int i,j;
	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 4; ++j) {
			state[j][i] = sbox[(unsigned char)state[j][i]];
		}
	}
}

/* invSubBytes: Replace each byte in the state by its corresponding value in the reversed
 * S-box.
 * From https://en.wikipedia.org/wiki/Advanced_Encryption_Standard#The_SubBytes_step
 */
static void invSubBytes(state_t state) {
	int i,j;
	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 4; ++j) {
			state[j][i] = inv_sbox[(unsigned char)state[j][i]];
		}
	}
}

/* shiftRows: Shift the stat rows, like a staircase.
 * From https://en.wikipedia.org/wiki/Advanced_Encryption_Standard#The_ShiftRows_step
 */
static void shiftRows(state_t state) {
	char temp;

	// Row 0 doesn't change

	// Row 1 rotate 1 column to the left.
	// [1,2,3,4] => [2,3,4,1]
	temp        = state[0][1];
	state[0][1] = state[1][1];
	state[1][1] = state[2][1];
	state[2][1] = state[3][1];
	state[3][1] = temp;

	// Row 2 rotate 2 column to the left
	// [1,2,3,4] => [3,4,1,2]
	// switch 1 and 3
	temp        = state[0][2];
	state[0][2] = state[2][2];
	state[2][2] = temp;
	// switch 2 and 4
	temp        = state[1][2];
	state[1][2] = state[3][2];
	state[3][2] = temp;

	// Row 3 rotate 3 column to the left, it's the same as rotating 1 column to
	// the right.
	// [1,2,3,4] => [4,1,2,3]
	temp        = state[3][3];
	state[3][3] = state[2][3];
	state[2][3] = state[1][3];
	state[1][3] = state[0][3];
	state[0][3] = temp;
}

/* invShiftRows: Inverse the shift step.
 * From https://en.wikipedia.org/wiki/Advanced_Encryption_Standard#The_ShiftRows_step
 */
static void invShiftRows(state_t state) {
	char temp;

	// Row 0 doesn't change

	// Row 1 rotate 1 column to the right 
	// [1,2,3,4] => [4,1,2,3]
	temp        = state[3][1];
	state[3][1] = state[2][1];
	state[2][1] = state[1][1];
	state[1][1] = state[0][1];
	state[0][1] = temp;

	// Row 2 rotate 2 column to the right
	// [1,2,3,4] => [3,4,1,2]
	// switch 1 and 3
	temp        = state[0][2];
	state[0][2] = state[2][2];
	state[2][2] = temp;
	// switch 2 and 4
	temp        = state[1][2];
	state[1][2] = state[3][2];
	state[3][2] = temp;

	// Row 3 rotate 3 column to the right, it's the same as rotating 1 column to
	// the left.
	// [1,2,3,4] => [2,3,4,1]
	temp           = state[0][3];
	state[0][3] = state[1][3];
	state[1][3] = state[2][3];
	state[2][3] = state[3][3];
	state[3][3] = temp;
}

/* xorIV: Simple XOR function to modify the 16 char buffer inline.
 * params:
 *     buffer: Buffer to modify
 *     iv:     Vector to xor
 */
static void xorIV(char *buffer, const char *iv){
	int x;
	for(x = 0; x < 16; ++x){
		// buffer is modified, no need to return it.
		buffer[x] ^= iv[x];
	}
}

/* invCipher: Main function of the inversion, perform all the rounds (initial,
 * middle and final rounds). This will work on a 16 bytes string and will be
 * called for each 16 bytes words in the encrypted string.
 * From https://en.wikipedia.org/wiki/Advanced_Encryption_Standard#High-level_description_of_the_algorithm
 * params:
 *     input:        The current input to decrypt
 *     input_length: The current input length
 *     key:          The key to use to decrypt
 *     result:       The result to fill with the decrypted string
 */
static void invCipher(const char* input, const int input_length, const char* key, char *result) {
	int round = 0;

	// Current state of the encryption/decryption flow
	static state_t* state;

	// The expanded key, look the article for size explanation, it's the `b`
	// variable in the article.
	// From https://en.wikipedia.org/wiki/Rijndael_key_schedule#Constants
	char expanded_key[176];

	// Duplicate the input into the result buffer
	memcpy(result, input, input_length);

	// The initial state is the first 16 bytes of the result array.
	state = (state_t*)result;

	keyExpansion(key, expanded_key);

	// Add the first expanded key to the state.
	addRoundKey(*state, ROUNDS_NUMBER, expanded_key);

	// Looping backward to undo all the decrypting. Will be looping ROUNDS_NUMBER
	// time.
	for (round = (ROUNDS_NUMBER - 1); round > 0; --round) {
		invShiftRows(*state);
		invSubBytes(*state);
		addRoundKey(*state, round, expanded_key);
		invMixColumns(*state);
	}

	// Perform the last round
	invShiftRows(*state);
	invSubBytes(*state);
	addRoundKey(*state, 0, expanded_key);
}

/* cipher: Main function of the encryption, perform all the rounds (initial,
 * middle and final rounds). This will work on a 16 bytes string and will be
 * called for each 16 bytes words in the encrypted string.
 * From https://en.wikipedia.org/wiki/Advanced_Encryption_Standard#High-level_description_of_the_algorithm
 * params:
 *     input:        The current input to encrypt
 *     input_length: The current input length
 *     key:          The key to use to encrypt
 *     result:       The result to fill with the encrypted string
 */
static void cipher(const char* input, const int input_length, const char* key, char *result) {
	int round = 0;

	// Current state of the encryption/decryption flow
	static state_t* state;

	// The expanded key, look the article for size explanation, it's the `b`
	// variable in the article.
	// From https://en.wikipedia.org/wiki/Rijndael_key_schedule#Constants
	char expanded_key[176];

	// Dupplicate the input into the result buffer
	memcpy(result, input, input_length);

	// The initial state is the first 16 bytes of the result array.
	state = (state_t*)result;

	keyExpansion(key, expanded_key);

	// Add the first expanded key to the state.
	addRoundKey(*state, 0, expanded_key);

	// Looping backward to undo all the decrypting. Will be looping ROUNDS_NUMBER
	// time.
	for (round = 1; round < ROUNDS_NUMBER; ++round) {
		subBytes(*state);
		shiftRows(*state);
		mixColumns(*state);
		addRoundKey(*state, round, expanded_key);
	}

	// Perform the last round
	subBytes(*state);
	shiftRows(*state);
	addRoundKey(*state, ROUNDS_NUMBER, expanded_key);
}

/* AES_CBC_decrypt: Entry point of the decryption flow in CBC mode. Will call
 * the invCipher function for every 16-bytes words in the input string.
 * params:
 *     input:        The current input to decrypt
 *     input_length: The current input length
 *     key:          The key to use to decrypt
 *     iv:           The initialisation vector to use to decrypt
 *     result:       The result to fill with the decrypted string
 */
void AES_CBC_decrypt(const char* input, const int input_length, const char* key, const char *iv, char **result){
	int i;

	// Duplicate the IV to allow changes during the flow.
	char current_iv[16];
	memcpy(current_iv, iv, 16);

	// Allocate the result buffer
	(*result) = (char*)malloc(input_length);

	// Decrypt word by word
	for(i = 0; i < input_length/16; ++i) {
		invCipher(input + (i*16), 16, key, (*result)+(i*16));

		// XOR the output against the IV
		xorIV((*result)+(i*16), current_iv);

		// The new IV is the current input string
		memcpy(current_iv, input+(i*16), 16);
	}
}

/* AES_ECB_encrypt: Entry point of the encryption flow in CBC mode. Will call
 * the cipher function for every 16-bytes words in the input string.
 * params:
 *     input:        The current input to encrypt
 *     input_length: The current input length
 *     key:          The key to use to encrypt
 *     iv:           The initialisation vector to use to encrypt
 *     result:       The result to fill with the encrypted string
 */
void AES_CBC_encrypt(const char* input, const int input_length, const char* key, const char *iv, char **result){
	int i;

	// Duplicate the current input bytes to allow changes.
	char current_input[16];

	// Duplicate the IV to allow changes during the flow.
	char current_iv[16];
	memcpy(current_iv, iv, 16);

	// Allocate the result buffer
	(*result) = (char*)malloc(input_length);

	// Decrypt word by word
	for(i = 0; i < input_length/16; ++i) {
		// Copy the current input bytes and xor them against the current IV.
		memcpy(current_input, input, 16);
		xorIV(current_input, current_iv);

		// Apply the cipher
		cipher(current_input, 16, key, (*result)+(i*16));
		
		// The new IV is the string just computed.
		memcpy(current_iv, (*result)+(i*16), 16);
	}
}

/* AES_ECB_decrypt: Entry point of the Decryption flow in ECB mode. Will call
 * the invCipher function for every 16-bytes words in the input string.
 * params:
 *     input:        The current input to decrypt
 *     input_length: The current input length
 *     key:          The key to use to decrypt
 *     result:       The result to fill with the decrypted string
 */
void AES_ECB_decrypt(const char* input, const int input_length, const char* key, char **result) {
	int i;

	// Allocate the result buffer
	(*result) = (char*)malloc(input_length);

	// Decrypt word by word
	for(i = 0; i < input_length/16; ++i) {
		invCipher(input + (i*16), 16, key, (*result)+(i*16));
	}
}

/* AES_ECB_encrypt: Entry point of the encryption flow in ECB mode. Will call
 * the cipher function for every 16-bytes words in the input string.
 * params:
 *     input:        The current input to encrypt
 *     input_length: The current input length
 *     key:          The key to use to encrypt
 *     result:       The result to fill with the encrypted string
 */
void AES_ECB_encrypt(const char* input, const int input_length, const char* key, char **result) {
	int i;

	// Allocate the result buffer
	(*result) = (char*)malloc(input_length);

	// Decrypt word by word
	for(i = 0; i < input_length/16; ++i) {
		cipher(input + (i*16), 16, key, (*result)+(i*16));
	}
}

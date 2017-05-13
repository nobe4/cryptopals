#ifndef XOR_H
#define XOR_H

int xor(const char* string1, const int str1_len, const char* string2, const int str2_len, char** result);
char detectSingleByteXORKey(const char* original, const int org_len, int(*score_function)(const char*));
int detectXORKeyLen(const char* string, const int str_len, const int min_key_length, const int max_key_len);

#endif // XOR_H

#ifndef MISC_H
#define MISC_H

#include "misc.h"

int englishScore(const char* string);
int hammingDistance(const char* string1, const int str1_len, const char* string2, const int str2_len);
int transposeByBlocks(const char* string, const int str_len, const int block_number, char ***blocks);
void removeCharFromString(char* string, char theChar);
void surroundString(const char *original, const int org_len, const char *prefix, const int pre_len, const char *suffix, const int suf_len, char **result);
void repeatString(const char *input, const int in_len, const int repeat, char **result);

#endif // MISC_H

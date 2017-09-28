#ifndef MISC_H
#define MISC_H

#include "misc.h"

int englishScore(const char* string);
int hammingDistance(const char* string1, const int str1_len, const char* string2, const int str2_len);
int transposeByBlocks(const char* string, const int str_len, const int block_number, char ***blocks);
void removeCharFromString(char* string, char theChar);

#endif // MISC_H

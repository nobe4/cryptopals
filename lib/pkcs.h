#ifndef PKCS_H
#define PKCS_H

int pkcs7Padding(const char *original, const int required_size, char **result);

#endif

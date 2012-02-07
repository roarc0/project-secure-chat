#include <openssl/evp.h>

#ifndef CRYPT_H
#define CRYPT_H

void printbyte(char);
void select_random_key(char*, int);

char *encrypt(int*, const char*, int, const char *, int);
char *decrypt(const char *, int, const char *, int);

#endif

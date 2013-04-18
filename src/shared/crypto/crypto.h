#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <cstdlib>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include "../utility/bytebuffer.h"

const int BlockSize=16;

using namespace std;

void printbyte(char);

int AesEncrypt(const ByteBuffer &key,
           const std::string &plaintext,
           ByteBuffer &ciphertext);

int AesDecrypt(const ByteBuffer &key,
           const ByteBuffer &ciphertext,
           std::string &plaintext);

#endif




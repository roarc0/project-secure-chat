#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <string>

#include "utility/bytebuffer.h"

#define BLOCK_SIZE 16

int GenerateSessionKey(ByteBuffer &key, int size);

int AesEncrypt(const ByteBuffer &key,
               const std::string &plaintext,
               ByteBuffer &ciphertext);

int AesDecrypt(const ByteBuffer &key,
               const ByteBuffer &ciphertext,
               std::string &plaintext);

#endif




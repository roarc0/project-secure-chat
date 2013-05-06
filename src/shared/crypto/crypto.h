#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include "utility/bytebuffer.h"
#include "base64.h"
#include <string>

#define BLOCK_SIZE 16

void SHA256_digest(const char* data, int length,
                   string& digest);

int  GenerateRandomKey(ByteBuffer &key, int size);

void Xor(ByteBuffer& data, ByteBuffer key);

int  AesEncrypt(const ByteBuffer &key,
               const ByteBuffer &plaintext,
               ByteBuffer &ciphertext);

int  AesDecrypt(const ByteBuffer &key,
               const ByteBuffer &ciphertext,
               ByteBuffer &plaintext);

int RsaEncrypt(const std::string key,
               const ByteBuffer &plaintext,
               ByteBuffer &ciphertext);

#endif




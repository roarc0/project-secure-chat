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
/*
class CryptoException{};

class Crypto
{
    ByteBuffer symKey;
    
    public:
    Crypto (ByteBuffer k): symKey(k)
    {
        OpenSSL_add_all_algorithms();
        OpenSSL_add_all_ciphers();
        OpenSSL_add_all_digests();
    }

    void setSymKey(ByteBuffer);
    ByteBuffer getSymKey();
    
    int AesEncrypt(const ByteBuffer &key,
               const std::string &plaintext,
               ByteBuffer &ciphertext);
    
    int AesDecrypt(const ByteBuffer &key,
               const ByteBuffer &ciphertext,
               std::string &plaintext);
    
    //ByteBuffer SymEncrypt(const std::string);
    //smartBuffer SymDecrypt(const ByteBuffer);
    //smartBuffer AsymEncrypt(smartBuffer);
    //smartBuffer AsymDecrypt(smartBuffer);
};
*/

int AesEncrypt(const ByteBuffer &key,
           const std::string &plaintext,
           ByteBuffer &ciphertext);

int AesDecrypt(const ByteBuffer &key,
           const ByteBuffer &ciphertext,
           std::string &plaintext);

#endif




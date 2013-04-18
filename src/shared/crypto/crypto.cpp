#include <cstring>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <vector>

#include "crypto.h"

using namespace std;

void printbyte(char b)
{
    printf("%X%X:", b >> 4 & 15, b & 15);
}

/******************* crypto *******************/

void Crypto::setSymKey(ByteBuffer key)
{
    symKey = key;
}

ByteBuffer Crypto::getSymKey()
{
    return symKey;
}
    
int Crypto::AesEncrypt(const ByteBuffer &key,
               const std::string &plaintext,
               ByteBuffer &ciphertext)
{
    unsigned char init[BlockSize];
    EVP_CIPHER_CTX ctx;
    const EVP_CIPHER *chp=0;
    int retval=0;
    unsigned char *buffer=0;

    if (key.size()==16)
    {
            chp = EVP_aes_128_cbc();
    }
    else if (key.size()==32)
    {
            chp = EVP_aes_256_cbc();
    }
    else
            return -1;

    RAND_pseudo_bytes(init, BlockSize);
    //ciphertext.assign((char *)init, ((char *)init)+BlockSize);
   
    EVP_CIPHER_CTX_init(&ctx);
    EVP_EncryptInit_ex(&ctx, chp, 0, (unsigned char *)key.contents(), init);

    int len = plaintext.length()+2*BlockSize;
    buffer = new unsigned char[len];
    retval = EVP_EncryptUpdate(
            &ctx,
            buffer,
            &len, 
            (unsigned char *)plaintext.c_str(),
            plaintext.length());

    if (retval < 0)
    {
            delete[] buffer;
            return retval;
    }

    ciphertext.append((char *)buffer, len); // ((char *)buffer)+len);
    delete[] buffer;
    
    len = 2*BlockSize;
    buffer = new unsigned char[len];
    retval = EVP_EncryptFinal_ex(
            &ctx,
            buffer,
            &len);

    if (retval<0)
    {
            delete[] buffer;
            return retval;
    }

    ciphertext.append((char *)buffer, len); //((char *)buffer)+len);
    delete[] buffer;

    EVP_CIPHER_CTX_cleanup(&ctx);
    return 0;
}

int Crypto::AesDecrypt(const ByteBuffer &key,
               const ByteBuffer &ciphertext,
               std::string &plaintext)
{
        unsigned char init[BlockSize];
        EVP_CIPHER_CTX ctx;
        const EVP_CIPHER *chp=0;
        int retval=0;
        unsigned char *buffer=0;
        
        if (key.size()==16)
        {
                chp = EVP_aes_128_cbc();
        }
        else if (key.size()==32)
        {
                chp = EVP_aes_256_cbc();
        } else
        {
                return -1;
        }

        const char *buf = (const char*)ciphertext.contents();
        for (int i=0; i<BlockSize; i++)
        {
                init[i] = *buf;
                buf++;
        }
 
        EVP_CIPHER_CTX_init(&ctx);
        EVP_DecryptInit_ex(&ctx, chp, 0, (unsigned char *)key.contents(), init);

        int len = ciphertext.size() - BlockSize;
        buffer = new unsigned char[len];
        retval = EVP_DecryptUpdate(
                &ctx,
                buffer,
                &len, 
                ((unsigned char *)ciphertext.contents()) + BlockSize,
                len);

       
        if (retval<0)
        {
                delete[] buffer;
                return retval;
        }

        plaintext.append((char *)buffer, ((char *)buffer)+len);
        delete[] buffer;

        len = 2*BlockSize;
        buffer = new unsigned char[len];
        retval = EVP_DecryptFinal_ex(
                &ctx,
                buffer,
                &len);

        if (retval<0)
        {
                delete[] buffer;
                return retval;
        }

        plaintext.append((char *)buffer, ((char *)buffer)+len);
        delete[] buffer;

        EVP_CIPHER_CTX_cleanup(&ctx);
        return 0;
}

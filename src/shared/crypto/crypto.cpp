#include "crypto.h"
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <iomanip>

void SHA256_digest(const char* data, int length, string& digest)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data, length);
    SHA256_Final(hash, &ctx);
    
    stringstream ss;

    ss.fill('0');
    ss << std::hex;
    for(int i=0; i<SHA256_DIGEST_LENGTH; i++)
        ss << std::setw(2) << (int) hash[i];
    digest = ss.str();
}

int GenerateRandomKey(ByteBuffer &key, int size)
{
    uint8 *buf;

    if (size != 16 && size != 32)
    {
        errno = EINVAL;
        return -1;
    }
    
    buf = new uint8[size];
    RAND_bytes(buf, size);
    key.clear();
    key.append(buf, size);
    
    return 0;
}

// funzione per generare la chiave di sessione a partire da Ka,Kb
void Xor(ByteBuffer& data, ByteBuffer key)
{
    uint16 k = 0;
 
    for(int i = 0; i < data.size(); i++)
    {
        data.contents()[i] = (uint8)(data.contents()[i] ^ key[k]);
        k=(++k < key.size() ? k:0);
    }
}

int AesEncrypt(const ByteBuffer &key,
               const ByteBuffer &plaintext,
               ByteBuffer &ciphertext)
{
    unsigned char init[BLOCK_SIZE];
    EVP_CIPHER_CTX ctx;
    const EVP_CIPHER *chp=0;
    int retval=0;
    unsigned char *buffer=0;

    if (key.size()==17)
    {
        chp = EVP_aes_128_cbc();
    }
    else if (key.size()==33)
    {
        chp = EVP_aes_256_cbc();
    }
    else
    {
        INFO("debug","CRYPTO: wrong key size -> %d\n", key.size());
        return -1;
    }

    RAND_pseudo_bytes(init, BLOCK_SIZE);
    ciphertext.append((char *)init, BLOCK_SIZE);
   
    EVP_CIPHER_CTX_init(&ctx);
    EVP_EncryptInit_ex(&ctx, chp, 0, (unsigned char *)key.contents(), init);

    int len = plaintext.size()+2*BLOCK_SIZE;
    buffer = new unsigned char[len];
    retval = EVP_EncryptUpdate(
            &ctx,
            buffer,
            &len, 
            (unsigned char *)plaintext.contents(),
            plaintext.size());

    if (retval < 0)
    {
            delete[] buffer;
            INFO("debug","CRYPTO: encryption error\n");
            return retval;
    }

    if (len)
        ciphertext.append((char *)buffer, len);

    delete[] buffer;
    
    len = 2*BLOCK_SIZE;
    buffer = new unsigned char[len];
    retval = EVP_EncryptFinal_ex(
            &ctx,
            buffer,
            &len);

    if (retval<0)
    {
            delete[] buffer;
            INFO("debug","CRYPTO: encryption error while finalizing\n");
            return retval;
    }

    if (len)
        ciphertext.append((char *)buffer, len);

    delete[] buffer;

    EVP_CIPHER_CTX_cleanup(&ctx);
    return 0;
}

int AesDecrypt(const ByteBuffer &key,
               const ByteBuffer &ciphertext,
               ByteBuffer &plaintext)
{
        unsigned char init[BLOCK_SIZE];
        EVP_CIPHER_CTX ctx;
        const EVP_CIPHER *chp=0;
        int retval=0;
        unsigned char *buffer=0;
        
        if (key.size()==17)
        {
                chp = EVP_aes_128_cbc();
        }
        else if (key.size()==33)
        {
                chp = EVP_aes_256_cbc();
        } else
        {
                return -1;
        }

        const char *buf = (const char*)ciphertext.contents();
        for (int i=0; i<BLOCK_SIZE; i++)
        {
                init[i] = *buf;
                buf++;
        }
 
        EVP_CIPHER_CTX_init(&ctx);
        EVP_DecryptInit_ex(&ctx, chp, 0, (unsigned char *)key.contents(), init);

        int len = ciphertext.size() - BLOCK_SIZE;
        buffer = new unsigned char[len];
        retval = EVP_DecryptUpdate(
                &ctx,
                buffer,
                &len, 
                ((unsigned char *)ciphertext.contents()) + BLOCK_SIZE,
                len);

       
        if (retval<0)
        {
                delete[] buffer;
                return retval;
        }

        if (len)
            plaintext.append((char *)buffer, len);
        delete[] buffer;

        len = 2*BLOCK_SIZE;
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

        if (len)
            plaintext.append((char *)buffer, len);
        delete[] buffer;

        EVP_CIPHER_CTX_cleanup(&ctx);
        return 0;
}

#include <iostream>
#include <cstring>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include "crypto.h"
#include <vector>

using namespace std;

void printbyte(char b)
{
    printf("%X%X:", b >> 4 & 15, b & 15);
}

void smartBuffer::random()
{
    RAND_bytes(val, len); // qualcuno richiami il mersenne twister!
}

void smartBuffer::print()
{
    for (int i = 0; i < len; i++)
        printbyte(val[i]);
    printf("\n");
}

smartBuffer Crypto::sym_encrypt (smartBuffer plaintext)
{
	EVP_CIPHER_CTX* ctx = (EVP_CIPHER_CTX*)malloc(sizeof(EVP_CIPHER_CTX));     
    EVP_CIPHER_CTX_init(ctx);      
    EVP_EncryptInit(ctx, EVP_des_ecb(), symKey.val, NULL);    
    int block_size = EVP_CIPHER_CTX_block_size(ctx);
    
    Buffer* ciphertext = new Buffer[plaintext.len + block_size + 1];
    int nc = 0, cursor = 0;
    EVP_EncryptUpdate(ctx, ciphertext, &nc, plaintext.val, plaintext.len+1);  
    cursor += nc;
      
    EVP_EncryptFinal(ctx, &ciphertext[cursor], &nc);
    cursor += nc;
    
    return smartBuffer(ciphertext, cursor);
}


smartBuffer Crypto::sym_decrypt (smartBuffer ciphertext)
{
	EVP_CIPHER_CTX* ctx = (EVP_CIPHER_CTX*)malloc(sizeof(EVP_CIPHER_CTX));     
    EVP_CIPHER_CTX_init(ctx);  
    EVP_DecryptInit(ctx, EVP_des_ecb(), symKey.val, NULL);
    int block_size = EVP_CIPHER_CTX_block_size(ctx);
    Buffer* plaintext = new Buffer[ciphertext.len + block_size + 1];
    
    int nc = 0, cursor = 0, ct_len = ciphertext.len;
    EVP_DecryptUpdate(ctx, &plaintext[cursor], &nc, ciphertext.val, ct_len);
    cursor += nc;
    EVP_DecryptFinal(ctx, &plaintext[cursor], &nc);
    cursor += nc;
    
    return smartBuffer(plaintext,cursor);
}


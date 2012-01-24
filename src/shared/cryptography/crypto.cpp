#include "crypt.h"

void printbyte(char b)
{
    char c;

    c = b;
    c = c >> 4;
    c = c & 15;
    printf("%X", c);
    c = b;
    c = c & 15;
    printf("%X:", c);
}

void select_random_key(char *k, int b)
{
    int i;
    RAND_bytes(k, b);
    for (i = 0; i < b - 1; i++)
        printbyte(k[i]);
    printbyte(k[b-1]);
    printf("\n");
}

EVP_CIPHER_CTX *init_ctx(const char *key)
{
    if (!key)
        return 0;

    EVP_CIPHER_CTX *ctx = (EVP_CIPHER_CTX *) malloc(sizeof(EVP_CIPHER_CTX));
    EVP_CIPHER_CTX_init(ctx);
    EVP_EncryptInit(ctx, EVP_des_ecb(), NULL, NULL); /* al momento solo ecb */

    printf("key size %d\n", EVP_CIPHER_key_length(EVP_des_ecb()));
    printf("block size %d\n", EVP_CIPHER_CTX_block_size(ctx));

    EVP_EncryptInit(ctx, NULL, key, NULL);

    return ctx;
}

char *
encrypt(int *ct_len,
        const char *plaintext, int pt_len,
        const char *key,       int key_size)
{
    char *ciphertext;
    int nc, nctot, i;
    int ct_ptr, pt_ptr, n;

    if ( !plaintext || !key)
    {
        return 0;
    }

    EVP_CIPHER_CTX *ctx = init_ctx(key);

    n = 10;
    nc = 0;
    nctot = 0;
    ct_ptr = pt_ptr = 0;

    for (i = 0; i < pt_len / n; i++)
    {
        EVP_EncryptUpdate(ctx, &ciphertext[ct_ptr], &nc, &plaintext[pt_ptr], n);
        ct_ptr += nc;
        pt_ptr += n;
        nctot += nc;
    }

    if ( pt_len % n )
    {
        EVP_EncryptUpdate(ctx, &ciphertext[ct_ptr], &nc,
        &plaintext[pt_ptr], pt_len % n);
        ct_ptr += nc;
        pt_ptr += pt_len % n;
        nctot += nc;
    }

    EVP_EncryptFinal(ctx, &ciphertext[ct_ptr], &nc);

    nctot += nc;

    for (i = 0; i <= *ct_len - 1; i++)
        printbyte(ciphertext[i]);
    printf("\n");

    return ciphertext;
}

char *decrypt(const char *ciphertext, int ct_len,
              const char *key,       int key_size)
{
    char *plaintext;
    int nc, nctot, i;
    int ct_ptr, pt_ptr, n;

    if ( !ciphertext || !key)
    {
        return 0;
    }

    EVP_CIPHER_CTX *ctx = init_ctx(key);

    plaintext = (char *)malloc(ct_len);
    bzero(plaintext, ct_len);

    EVP_CIPHER_CTX_init(ctx);
    EVP_DecryptInit(ctx, EVP_des_ecb(), key, NULL);

    nc = 0;
    ct_ptr = 0;
    EVP_DecryptUpdate(ctx, &plaintext[ct_ptr],
    &nc, &ciphertext[ct_ptr], ct_len);
    EVP_DecryptFinal(ctx, &plaintext[ct_ptr],
    &nc);

    for (i = 0; i <= ct_len - 1; i++)
        printf("%c:", plaintext[i]);
    printf("%s\n", plaintext);

    return plaintext;
}

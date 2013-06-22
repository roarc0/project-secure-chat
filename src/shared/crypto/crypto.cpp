#include "crypto.h"

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

#include <iomanip>

void CryptoInit()
{
    INFO("debug", "CRYPTO: %s\n", SSLeay_version(SSLEAY_VERSION));
    OpenSSL_add_all_algorithms();
}

void SHA256_digest(const string& data, string& digest)
{
    SHA256_digest(data.c_str(), data.length(), digest);
}

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
    
    delete[] buf;
    return 0;
}

int GenerateRandomData(ByteBuffer &key, int size)
{
    uint8 *buf;
    
    buf = new uint8[size];
    RAND_pseudo_bytes(buf, size);
    key.clear();
    key.append(buf, size);
    
    delete[] buf;
    return 0;
}

// funzione per generare la chiave di sessione a partire da Ka,Kb
void Xor(ByteBuffer& data, const ByteBuffer& key)
{
    uint16 k = 0;
 
    for(uint32 i = 0; i < uint32(data.size()); i++)
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
    int ret=0;
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
    {
        INFO("debug","CRYPTO: wrong key size -> %d\n", key.size());
        return -1;
    }

    if (!plaintext.size())
    {
        INFO("debug","CRYPTO: wrong plaintext size!\n");
        return -1;
    }

    RAND_pseudo_bytes(init, BLOCK_SIZE);
    ciphertext.append((char *)init, BLOCK_SIZE);
   
    EVP_CIPHER_CTX_init(&ctx);
    EVP_EncryptInit_ex(&ctx, chp, 0, (unsigned char *)key.contents(), init);

    int len = plaintext.size()+2*BLOCK_SIZE;
    buffer = new unsigned char[len];
    ret = EVP_EncryptUpdate(
            &ctx,
            buffer,
            &len, 
            (unsigned char *)plaintext.contents(),
            plaintext.size());

    if (ret < 0)
    {
            delete[] buffer;
            INFO("debug","CRYPTO: encryption error\n");
            return ret;
    }

    if (len)
        ciphertext.append((char *)buffer, len);

    delete[] buffer;
    
    len = 2*BLOCK_SIZE;
    buffer = new unsigned char[len];
    ret = EVP_EncryptFinal_ex(
            &ctx,
            buffer,
            &len);

    if (ret<0)
    {
            delete[] buffer;
            INFO("debug","CRYPTO: encryption error while finalizing\n");
            return ret;
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
        int ret=0;
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
        {
            INFO("debug","CRYPTO: wrong key size -> %d\n", key.size());
            return -1;
        }

        if (ciphertext.size() % BLOCK_SIZE)
        {
            INFO("debug","CRYPTO: wrong ciphertext size!\n");
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
        ret = EVP_DecryptUpdate(
                &ctx,
                buffer,
                &len, 
                ((unsigned char *)ciphertext.contents()) + BLOCK_SIZE,
                len);

       
        if (ret<0)
        {
                delete[] buffer;
                return ret;
        }

        if (len)
            plaintext.append((char *)buffer, len);
        delete[] buffer;

        len = 2*BLOCK_SIZE;
        buffer = new unsigned char[len];
        ret = EVP_DecryptFinal_ex(
                &ctx,
                buffer,
                &len);

        if (ret<0)
        {
                delete[] buffer;
                return ret;
        }

        if (len)
            plaintext.append((char *)buffer, len);
        delete[] buffer;

        EVP_CIPHER_CTX_cleanup(&ctx);
        return 0;
}

void RsaPrintError()
{
    char str2[256];  /* Horror Show */
    ERR_load_crypto_strings();
    ERR_error_string(ERR_get_error(), str2);
    INFO("debug","CRYPTO: %s\n", str2);
    ERR_free_strings();
}

int RsaKeySize(const char* str, const char* password, bool pub)
{
    assert(str);
    RSA * key = NULL;

    FILE* fp = fopen(str, "rb");

    if(!fp)
    {
        INFO("debug", "CRYPTO: error opening \"%s\"\n", str);
        perror("CRYPTO: fopen");
        return 0;
    }
    
    if (pub)
        PEM_read_RSA_PUBKEY(fp, &key, NULL, NULL);
    else
        PEM_read_RSAPrivateKey(fp, &key, NULL, (void*) password);
    
    if(key)
        INFO("debug","CRYPTO: %d bit private key loaded\n", RSA_size(key) * 8);
    else
        RsaPrintError();
    
    int size = RSA_size(key);
    RSA_free(key);
    fclose(fp);
    
    return size;
}

RSA* RsaPubKey(const char* str)
{
    assert(str);
    RSA * key = NULL;
    const char* key_prefix = "-----BEGIN PUBLIC KEY-----";

    if (strncmp(str, key_prefix, strlen(key_prefix)) == 0)
    {
        BIO *bio = BIO_new_mem_buf((void*)str, -1);
        INFO("debug","CRYPTO: reading public key from string\n");
        key = PEM_read_bio_RSA_PUBKEY(bio, 0, 0, 0); 
        BIO_free(bio);
    }
    else
    {
        FILE* fp = fopen(str, "rb");
   
        if(!fp)
        {
            INFO("debug", "CRYPTO: error opening \"%s\"\n", str);
            perror("CRYPTO: fopen");
            return NULL;
        }

        INFO("debug","CRYPTO: reading public key from file: \"%s\"\n", str);
        PEM_read_RSA_PUBKEY(fp, &key, NULL, NULL);
        
        fclose(fp);
    }
    
    if(key)
        INFO("debug","CRYPTO: %d bit public key loaded\n", RSA_size(key) * 8);
    else
        RsaPrintError();        
    
    return key;
}


RSA* RsaPrivKey(const char* str, const char* password = NULL)
{
    assert(str);
    RSA * key = NULL;
        
    FILE* fp = fopen(str, "rb");

    if(!fp)
    {
        INFO("debug", "CRYPTO: error opening \"%s\"\n", str);
        perror("CRYPTO: fopen");
        return NULL;
    }
    
    INFO("debug","CRYPTO: reading private key from file: \"%s\"\n", str);
    PEM_read_RSAPrivateKey(fp, &key, NULL, (void*) password);
    
    if(key)
        INFO("debug","CRYPTO: %d bit private key loaded\n", RSA_size(key) * 8);
    else
        RsaPrintError();
  
    fclose(fp);

    return key;
}

int RsaEncrypt(const std::string key_str,
               const ByteBuffer &plaintext,
               ByteBuffer &ciphertext)
{
    int ret = 0;
    RSA *key = RsaPubKey(key_str.c_str());
    unsigned char *buf;
    
    if(!key)
    {
        INFO("debug","CRYPTO: RSA error reading public key\n");
        return -1;
    }

    buf = (unsigned char*) malloc(RSA_size(key));
       
    if((ret = RSA_public_encrypt(plaintext.size(),
                                 (unsigned char*)plaintext.contents(),
                                 buf, key, RSA_PKCS1_OAEP_PADDING)) == -1)
    {
        RsaPrintError();
    }
   else
    {
        ciphertext.clear();
        ciphertext.append(buf, ret);
        ret = 0;
    }

    RSA_free(key);
    return ret;
}

int RsaDecrypt(const std::string key_str,
               const char* password,
               const ByteBuffer &ciphertext,
               ByteBuffer &plaintext)
{
    int ret = 0;
    RSA *key;
    unsigned char *buf;
    
    if (!password || strlen(password) == 0)
        key = RsaPrivKey(key_str.c_str(), NULL);
    else
        key = RsaPrivKey(key_str.c_str(), password);
    
    if(!key)
    {
        INFO("debug","CRYPTO: RSA error reading private key\n");
        return -1;
    }

    buf = (unsigned char*) malloc(RSA_size(key));

    if((ret = RSA_private_decrypt(ciphertext.size(),
                                 (unsigned char*)ciphertext.contents(),
                                 buf, key, RSA_PKCS1_OAEP_PADDING)) == -1)
    {
        RsaPrintError();
    }
    else
    {
        plaintext.clear();
        plaintext.append(buf, ret);
        ret = 0;
    }   

    RSA_free(key);
    return ret;
}

int RsaSign(const std::string key_str,
            const char* password,
            const ByteBuffer &data,
            ByteBuffer &sign)
{
    int ret = 0;
    RSA *key;
    
    if (!password || strlen(password) == 0)
        key = RsaPrivKey(key_str.c_str(), NULL);
    else
        key = RsaPrivKey(key_str.c_str(), password);
    
    if(!key)
    {
        INFO("debug","CRYPTO: RSA error reading private key\n");
        return -1;
    }

    SHA256_CTX context;
    unsigned char digest[SHA256_DIGEST_LENGTH];

    SHA256_Init(&context);
    SHA256_Update(&context, (unsigned char*)data.contents(), data.size());
    SHA256_Final(digest, &context);
    
    int key_len = RSA_size(key);
    uint8* sig = (uint8*) malloc(sizeof(char) * key_len);
    uint32 sig_len = 0;

    if ( RSA_sign(NID_sha1, digest, SHA256_DIGEST_LENGTH, sig, &sig_len, key) == 0 )
    {
        INFO("debug","CRYPTO: RSA error, couldn't sign message digest.\n");
        RsaPrintError();
        return 1;
    }
    
    sign.append(sig, sig_len);

    free(sig);
    RSA_free(key);
    return 0;
}

int RsaVerify(const std::string key_str,
              const ByteBuffer &data,
              const ByteBuffer &sign)
{
    int ret = 0;
    RSA *key = RsaPubKey(key_str.c_str());
    unsigned char *buf;
    
    if(!key)
    {
        INFO("debug","CRYPTO: RSA error reading public key\n");
        return -1;
    }

    SHA256_CTX context;
    unsigned char digest[SHA256_DIGEST_LENGTH];

    SHA256_Init(&context);
    SHA256_Update(&context, (unsigned char*)data.contents(), data.size());
    SHA256_Final(digest, &context);
    
    int key_len = RSA_size(key);
    uint8* sig = (uint8*) sign.contents();
    uint32 sig_len = sign.size();
    
    if ( RSA_verify(NID_sha1, digest, SHA256_DIGEST_LENGTH,
                    sig, sig_len, key) == 0 )
    {
        INFO("debug","CRYPTO: RSA error, couldn't verify message digest.\n");
        RsaPrintError();
        return 1;
    }
    
    RSA_free(key);
    return 0;
}

bool RsaSignTest(const char* pem_file,
                 const char* pub_file,
                 const char* pwd)
{
    if(!pem_file || !pub_file)
        return false;
    
    ByteBuffer msg, sign;
    msg << "rsa message sign and verify test";
    RsaSign(pem_file, pwd, msg, sign);

    //INFO("debug", "sign of \"%s\" is\n", msg.contents());
    //sign.hexlike();
    
    return RsaVerify(pub_file, msg, sign) == 0;
}

bool RsaTest(const char* pem_file,
             const char* pub_file,
             const char* pwd)
{
    if(!pem_file || !pub_file)
        return false;
    
    ByteBuffer in, out, out2;
    in << "rsatest";
    RsaEncrypt(pub_file, in, out);
    RsaDecrypt(pem_file, pwd, out, out2);
   
    return in.compare(out2);
}

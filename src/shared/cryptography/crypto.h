#ifndef CRYPT_H
#define CRYPT_H
#include <iostream>
#include <string>
#include <openssl/evp.h>

typedef unsigned char Buffer;

using namespace std;
//{}
void printbyte(char);

class smartBuffer { // not so smart
    public:
    Buffer *const val;
    int len;

    smartBuffer(int l) : val(new Buffer[l]), len(l) { random(); };
    smartBuffer(char* v) : val((Buffer*)v), len(strlen(v)){};
    smartBuffer(string v) : val((Buffer*)v.c_str()), len(v.size()){};
    smartBuffer(Buffer* v = 0, int l = 0) : val(v),len(l){};
    
    void print();
    void random();

    std::ostream& operator<<(std::ostream& os) // non mi compilava
    {
       return cout << val;
    }
};

class CryptoException{};

class Crypto
{
    smartBuffer symKey, asymKey;
    
    public:
    Crypto (smartBuffer k = smartBuffer()): symKey(k){};

    void setSymKey(smartBuffer);
    smartBuffer getSymKey();
    
    smartBuffer sym_encrypt(smartBuffer);
    smartBuffer sym_decrypt(smartBuffer);
    smartBuffer asym_encrypt(smartBuffer);
    smartBuffer asym_decrypt(smartBuffer);
};

#endif




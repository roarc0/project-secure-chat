#ifndef CRYPT_H
#define CRYPT_H
#include <openssl/evp.h>
#include <string>

typedef unsigned char Buffer;
using namespace std;
//{}
void printbyte(char);

struct smartBuffer { // not so smart
    Buffer *const val;
    int len;
    smartBuffer(int l) : val(new Buffer[l]), len(l) { random(); };
    smartBuffer(char* v) : val((Buffer*)v), len(strlen(v)){};
    smartBuffer(string v) : val((Buffer*)v.c_str()), len(v.size()){};
    smartBuffer(Buffer* v = 0, int l = 0) : val(v),len(l){};
    
    void print();
    void random();
};

class CryptoException{};

class Crypto
{
    smartBuffer symKey, asymKey;
    
    public:
    Crypto (smartBuffer k = smartBuffer()): symKey(k){};
    
    smartBuffer sym_encrypt(smartBuffer);
    smartBuffer sym_decrypt(smartBuffer);
    smartBuffer asym_encrypt(smartBuffer);
    smartBuffer asym_decrypt(smartBuffer);
};

ostream& operator<<(ostream& o, smartBuffer s)
{
   o<<s.val;
}

#endif




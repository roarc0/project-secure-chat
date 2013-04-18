#include "packet.h"

//WIP
int Packet::Encrypt(ByteBuffer key)
{
    string plaintext;
    ByteBuffer ciphertext;
    int retval=0;

    key.clear();
    key << "11111222223333344444555556666677"; // 32 Byte = 256 Bit key.
    //plaintext << this; 
    //std::cout << plaintext << std::endl;
    retval = AesEncrypt(key, plaintext, ciphertext);
    
    if(!retval)
        m_encrypted = true;
    return retval;
}

int Packet::Decrypt(ByteBuffer key)
{
    ByteBuffer *ciphertext=this;
    std::string plaintext;
    int retval=0;

    key.clear();
    key << "11111222223333344444555556666677"; // 32 Byte = 256 Bit key.

    //std::cout << ciphertext << std::endl;
    retval = AesDecrypt(key, *ciphertext, plaintext);
    //std::cout << plaintext << std::endl;
    
    if (!retval)
        m_encrypted = false;
    return retval;
}

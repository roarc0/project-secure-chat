#include "packet.h"

//WIP
// usare bytebuffer anche per plaintext ( da cambiare funzioni di crypto.cpp )

int Packet::Encrypt(ByteBuffer key)
{
    string plaintext = (const char*) this->contents(); // usare direttamente il puntatore del ByteBuffer
    ByteBuffer ciphertext;
    int ret=0;

    std::cout << std::endl << "ENCRYPTING: " << plaintext << std::endl;
    ret = AesEncrypt(key, plaintext, ciphertext);
    ciphertext.hexlike();
    
    if(!ret)
    {
        INFO("debug", "PACKET: Encrypted\n");
        m_encrypted = true;
        this->clear();
        this->append(ciphertext.contents(), ciphertext.size());
    }
    else
        INFO("debug", "PACKET: Encryption failed\n");
    
    return ret;
}

int Packet::Decrypt(ByteBuffer key)
{
    ByteBuffer *ciphertext = (ByteBuffer*) this; // Mother Of God
    std::string plaintext;
    int ret=0;

    std::cout << std::endl << "DECRYPTING: " << std::endl;
    ciphertext->hexlike();

    ret = AesDecrypt(key, *ciphertext, plaintext);
    std::cout << std::endl << "DECRYPTED:  \"" << plaintext << "\""<< std::endl;
    
    if (!ret)
    {
        INFO("debug", "PACKET: Decrypted\n");
        m_encrypted = false;
        this->clear();
        this->append((uint8*)plaintext.c_str(), plaintext.length());
        
    }
    else
        INFO("debug", "PACKET: Decryption failed\n");
     
    return ret;
}

void Packet::Incapsulate(Packet& pkt)
{
    *this<<pkt.GetOpcode();
    *this<<pkt.size();
    this->append(pkt.contents(), pkt.size());
}

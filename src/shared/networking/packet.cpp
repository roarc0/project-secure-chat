#include "packet.h"

//WIP
// usare bytebuffer anche per plaintext ( da cambiare funzioni di crypto.cpp )

int Packet::Encrypt(ByteBuffer key)
{
    ByteBuffer ciphertext;
    int ret=0;

    ret = AesEncrypt(key, (ByteBuffer)(*this), ciphertext);
    ciphertext.hexlike();
    
    if(!ret)
    {
        INFO("debug", "PACKET: Encrypted\n");
        m_encrypted = true;
        this->clear();
        if (ciphertext.size())
            this->append(ciphertext.contents(), ciphertext.size());
    }
    else
        INFO("debug", "PACKET: Encryption failed\n");
    
    return ret;
}

int Packet::Decrypt(ByteBuffer key)
{
    ByteBuffer *ciphertext = (ByteBuffer*) this; // Mother Of God
    ByteBuffer plaintext;
    int ret=0;

    std::cout << std::endl << "DECRYPTING: " << std::endl;
    ciphertext->hexlike();

    ret = AesDecrypt(key, *ciphertext, plaintext);
    
    if (!ret)
    {
        INFO("debug", "PACKET: Decrypted\n");
        m_encrypted = false;
        this->clear();
        this->append(plaintext);
    }
    else
        INFO("debug", "PACKET: Decryption failed\n");
     
    return ret;
}

void Packet::Incapsulate(Packet& pkt)
{
    *this<<pkt.GetOpcode();
    *this<<pkt.size();
    if (pkt.size())
        this->append(pkt.contents(), pkt.size());
}

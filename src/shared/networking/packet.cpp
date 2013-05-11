#include "packet.h"

int Packet::Encrypt(ByteBuffer key)
{
    ByteBuffer ciphertext;
    int ret=0;

    ret = AesEncrypt(key, (ByteBuffer)(*this), ciphertext);
    ciphertext.hexlike();
    
    if (!ret)
    {
        INFO("debug", "PACKET: encrypted\n");
        m_encrypted = true;
        this->clear();
        if (ciphertext.size())
            this->append(ciphertext.contents(), ciphertext.size());
    }
    else
        INFO("debug", "PACKET: encryption failed\n");
    
    return ret;
}

int Packet::Decrypt(ByteBuffer key)
{
    ByteBuffer *ciphertext = (ByteBuffer*) this; // Mother Of God
    ByteBuffer plaintext;
    int ret=0;

    INFO("debug", "PACKET: decrypting\n");
    ciphertext->hexlike();

    ret = AesDecrypt(key, *ciphertext, plaintext);
    
    if (!ret)
    {
        INFO("debug", "PACKET: decrypted\n");
        m_encrypted = false;
        this->clear();
        this->append(plaintext);
    }
    else
        INFO("debug", "PACKET: decryption failed\n");
     
    return ret;
}

void Packet::Incapsulate(Packet& pkt)
{
    *this << uint16(pkt.GetOpcode());
    *this << uint16(pkt.size());
    INFO("debug","PACKET: packet incapsulate [header opcode %d, length %d]\n", pkt.GetOpcode(), pkt.size());
    if (pkt.size())
        this->append(pkt.contents(), pkt.size());
}

Packet* Packet::Decapsulate()
{    
    if (empty())
        return NULL;

    uint16 opcode; 
    uint16 size;
    *this >> opcode;
    *this >> size;

    INFO("debug","PACKET: packet decapsulate [header opcode %d, length %d]\n", opcode, size);

    Packet* new_pkt = new Packet(opcode, size);
    if (!new_pkt)
        return NULL;
    
    if (size)
    {
        new_pkt->append(contents()+rpos(), size);
        read_skip(size);
    }
    
    //INFO("debug","PACKET: packet decapsulate [contents %s]\n", new_pkt->contents());

    return new_pkt;
}

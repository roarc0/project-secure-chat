#include "packet.h"

/*Packet ForgePacket(unsigned short opcode, const char *data)
{
    Packet packet;

    if (data)
        packet.m_data = data;

    packet.SetOpcode(opcode);

    return packet;
}

void Packet::GetRawData(unsigned char* rawData)
{
    if (!rawData)
        return;

    unsigned short temp;

    temp = m_opcode;        // temp
    temp &= 0x00ff;
    rawData[0] = (unsigned char)temp;
    temp = m_opcode;
    temp = temp>>8;
    rawData[1] = (unsigned char)temp;

    temp = m_data.length();  // temp
    temp &= 0x00ff;
    rawData[2] = (unsigned char)temp;
    temp = m_data.length();
    temp = temp>>8;
    rawData[3] = (unsigned char)temp;
    
    memcpy((void*) (rawData + OPCODE_SIZE + LENGTH_SIZE), (void*) m_data.c_str(), m_data.length());
}

unsigned int Packet::GetRawLength()
{
    return m_data.length() + OPCODE_SIZE + LENGTH_SIZE;
}*/

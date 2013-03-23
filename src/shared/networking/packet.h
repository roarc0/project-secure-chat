#ifndef _PACKET_H
#define _PACKET_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../common.h"
#include "bytebuffer.h"

using namespace std;

#define   OPCODE_SIZE       sizeof(unsigned short)
#define   LENGTH_SIZE       sizeof(unsigned short)

enum eOpcode
{
    OP_NULL = 0,
    OP_LOGIN = 1,
    MAX_OPCODE = 2
};

class Packet : public ByteBuffer
{
    public:
        Packet() : ByteBuffer(0), m_opcode(OP_NULL)
        {
            gettimeofday(&m_createTime, NULL);
        }

        Packet(uint16 opcode, size_t res=200) : ByteBuffer(res), m_opcode(opcode) 
        {
            gettimeofday(&m_createTime, NULL);
        }

        void Initialize(uint16 opcode, size_t newres=200)
        {
            clear();
            _storage.reserve(newres);
            m_opcode = opcode;
        }

        // costruttore di copia
        Packet(const Packet &packet) : ByteBuffer(packet), m_opcode(packet.m_opcode), m_createTime(packet.m_createTime)
        {

        }

        uint32 GetSize()
        {
            return size() + OPCODE_SIZE + LENGTH_SIZE;
        }
 
        uint16 GetOpcode() const { return m_opcode; }
        void SetOpcode(uint16 opcode) { m_opcode = opcode; }

        void ResetTime() 
        {
            gettimeofday(&m_createTime, NULL);
        }

        uint32 GetTime() // In millisecondi
        {
            uint32 seconds  = m_createTime.tv_sec;  // - start.tv_sec;  TODO Quando è stato avviato il programma
            uint32 useconds = m_createTime.tv_usec; // - start.tv_usec; TODO Quando è stato avviato il programma
            return ((seconds) * 1000 + useconds/1000.0) + 0.5;
        }

    protected:
        uint16 m_opcode;
        timeval m_createTime;
};

#define HEADER_SIZE 4

struct PktHeader
{
    PktHeader(uint32 size, uint16 cmd)
    {
        uint8 headerIndex=0;
        header[headerIndex++] = 0xFF & cmd;
        header[headerIndex++] = 0xFF & (cmd>>8);
        
        header[headerIndex++] = 0xFF & (size>>8);
        header[headerIndex++] = 0xFF & size;        
    }

    PktHeader(char* buffer, uint8 size)
    {
        if (size > HEADER_SIZE)
            size = HEADER_SIZE;

        for (uint8 headerIndex = 0; headerIndex < size; headerIndex++)
            header[headerIndex] = buffer[headerIndex];
    }

    uint16 getSize()
    {
        return ((8<<uint16(header[2]))&0xFF00) | (uint16(header[3])&0x00FF);
    }

    uint16 getHeader()
    {
        return ((8<<uint16(header[1]))&0xFF00) | (uint16(header[0])&0x00FF);
    }

    uint8 getHeaderLength()
    {
        return HEADER_SIZE;
    }

    uint8 header[HEADER_SIZE];
};
#endif

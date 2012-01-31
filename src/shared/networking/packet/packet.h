#ifndef _PACKET_H
#define _PACKET_H

#include "bytebuffer.h"

class Packet : public ByteBuffer
{
    public:
        
		Packet() : ByteBuffer(0), m_opcode(0) {}
		
        explicit Packet(uint16 opcode, size_t res=200) : ByteBuffer(res), m_opcode(opcode) {}
        
		// costruttore di copia
        Packet(const Packet &packet) : ByteBuffer(packet), m_opcode(packet.m_opcode) {}

        void Initialize(uint16 opcode, size_t newres=200)
        {
            clear();
            _storage.reserve(newres);
            m_opcode = opcode;
        }

        uint16 GetOpcode() const { return m_opcode; }
        void SetOpcode(uint16 opcode) { m_opcode = opcode; }

    protected:
        uint16 m_opcode;
};
#endif

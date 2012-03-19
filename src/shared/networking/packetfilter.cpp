#include "packetfilter.h"
#include "opcode.h"

bool MapSessionFilter::Process(Packet* packet)
{
    OpcodeHandler const &opHandle = opcodeTable[packet->GetOpcode()];

    if (opHandle.packetProcessing == PROCESS_INPLACE)
        return true;

    if (opHandle.packetProcessing == PROCESS_THREADUNSAFE)
        return false;

    return m_pSession->IsInChannel();
}

bool SingleSessionFilter::Process(Packet* packet)
{
    OpcodeHandler const &opHandle = opcodeTable[packet->GetOpcode()];

    if (opHandle.packetProcessing == PROCESS_INPLACE)
        return true;

    if (opHandle.packetProcessing == PROCESS_THREADUNSAFE)
        return true;

    return (m_pSession->IsInChannel() == false);
}

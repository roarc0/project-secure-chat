#include "packetfilter.h"

bool CellSessionFilter::Process(Packet* packet)
{
    OpcodeHandler const &opHandle = opcodeTable[packet->GetOpcode()];

    if (opHandle.packetProcessing == PROCESS_INPLACE)
        return true;

    if (opHandle.packetProcessing == PROCESS_THREADUNSAFE)
        return false;

    return m_pSession->IsInCell();
}

bool SingleSessionFilter::Process(Packet* packet)
{
    OpcodeHandler const &opHandle = opcodeTable[packet->GetOpcode()];

    if (opHandle.packetProcessing == PROCESS_INPLACE)
        return true;

    if (opHandle.packetProcessing == PROCESS_THREADUNSAFE)
        return true;

    return (m_pSession->IsInCell() == false);
}

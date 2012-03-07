bool CellSessionFilter::Process(Packet* packet)
{
    OpcodeHandler const &opHandle = opcodeTable[packet->GetOpcode()];

    if (opHandle.packetProcessing == PROCESS_INPLACE)
        return true;

    if (opHandle.packetProcessing == PROCESS_THREADUNSAFE)
        return false;

    UserSession* m_pUses = m_pSession->GetUserSession();
    if (!m_pUses)
        return false;

    return m_pUses->IsInInstance();
}

bool SingleSessionFilter::Process(Packet* packet)
{
    OpcodeHandler const &opHandle = opcodeTable[packet->GetOpcode()];

    if (opHandle.packetProcessing == PROCESS_INPLACE)
        return true;

    if (opHandle.packetProcessing == PROCESS_THREADUNSAFE)
        return true;

    UserSession* m_pUses = m_pSession->GetUserSession();
    if (!m_pUses)
        return true;

    return (m_pUses->IsInInstance() == false);
}

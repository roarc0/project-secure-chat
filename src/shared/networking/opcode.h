#ifndef OPCODE_H
#define OPCODE_H

#include "session.h"

enum MessageTypes
{ 
    MSG_NULL_ACTION = 0,
    CMSG_PING,                                      
    SMSG_PING,
    SMSG_QUEUE_POSITION,
    SMSG_SYSTEM_MESSAGE,                                      
    CMSG_MESSAGE,                                  
    SMSG_MESSAGE,
    CMSG_WHISP,
    SMSG_WHISP,
    CMSG_CREATE_CHANNEL,               
    CMSG_JOIN_CHANNEL,  
    SMSG_JOIN_CHANNEL,                          
    CMSG_LEAVE_CHANNEL,
    SMSG_LEAVE_CHANNEL,                           
    CMSG_CHANNEL_LIST,
    CMSG_CHANNEL_INFO,       
    CMSG_LOGIN,
    SMSG_LOGIN,
    
    NUM_MSG_TYPES,                                  
};

enum SessionReqStatus
{
    STATUS_LOGGING = 0,                         // Collegato               
    STATUS_LOGGED,                              // Loggato           
    STATUS_TRANSFER,                            // Trasferimento      
    STATUS_LOGOUT,                              // Sta per sloggarsi
    STATUS_NEVER,                               // Non accettato 
    STATUS_UNHANDLED                            // Non valido
};

enum PacketProcessing
{
    PROCESS_INPLACE = 0,                         // il pacchetto è gestito in single o in multithread (a seconda del caso)
    PROCESS_THREADUNSAFE,                        // il pacchetto è gestito solo in singletread
    PROCESS_THREADSAFE                           // il pacchetto è gestito solo in multithread
};

struct OpcodeHandler
{
    char const* name;
    SessionReqStatus status;
    PacketProcessing packetProcessing;
    void (Session::*handler)(Packet& recvPacket);
};

extern OpcodeHandler opcodeTable[NUM_MSG_TYPES];

#endif

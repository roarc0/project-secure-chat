#ifndef OPCODE_H
#define OPCODE_H

#include "session.h"

enum MessageTypes
{ 
    MSG_NULL_ACTION = 0,
    CMSG_PING,                                      
    SMSG_PING,                                     
    CMSG_MESSAGE,                                  
    SMSG_MESSAGE,   
    SMSG_SYSTEM_MESSAGE,                                
    CMSG_JOIN_CHANNEL,                             
    CMSG_LEAVE_CHANNEL,                            
    SMSG_CHANNEL_NOTIFY,                           
    CMSG_CHANNEL_LIST,                             
    SMSG_CHANNEL_LIST,                           
    CMSG_CHANNEL_PASSWORD,                       
    CMSG_CHANNEL_SET_OWNER,                        
    CMSG_CHANNEL_OWNER,                         
    CMSG_CHANNEL_MODERATOR,                     
    CMSG_CHANNEL_UNMODERATOR,                     
    CMSG_CHANNEL_MUTE,                              
    CMSG_CHANNEL_UNMUTE,                          
    CMSG_CHANNEL_INVITE,                            
    CMSG_CHANNEL_KICK,                           
    CMSG_CHANNEL_BAN,                             
    CMSG_CHANNEL_UNBAN,                             
    CMSG_CHANNEL_ANNOUNCEMENTS,                     
    CMSG_CHANNEL_MODERATE,                          
    SMSG_QUEUE_POSITION,                            

    NUM_MSG_TYPES,                                  
};

enum SessionStatus
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
    SessionStatus status;
    PacketProcessing packetProcessing;
    void (Session::*handler)(Packet& recvPacket);
};

extern OpcodeHandler opcodeTable[NUM_MSG_TYPES];

#endif

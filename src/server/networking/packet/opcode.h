#ifndef OPCODE_H
#define OPCODE_H

#include "session.h"

enum MessageTypes
{ 
    MSG_NULL_ACTION                                 = 0x000,
    CMSG_MESSAGE                                    = 0x001,
    SMSG_MESSAGE                                    = 0x002,
    CMSG_JOIN_CHANNEL                               = 0x003,
    CMSG_LEAVE_CHANNEL                              = 0x004,
    SMSG_CHANNEL_NOTIFY                             = 0x005,
    CMSG_CHANNEL_LIST                               = 0x006,
    SMSG_CHANNEL_LIST                               = 0x007,
    CMSG_CHANNEL_PASSWORD                           = 0x008,
    CMSG_CHANNEL_SET_OWNER                          = 0x009,
    CMSG_CHANNEL_OWNER                              = 0x00A,
    CMSG_CHANNEL_MODERATOR                          = 0x00B,
    CMSG_CHANNEL_UNMODERATOR                        = 0x00C,
    CMSG_CHANNEL_MUTE                               = 0x00D,
    CMSG_CHANNEL_UNMUTE                             = 0x00E,
    CMSG_CHANNEL_INVITE                             = 0x00F,
    CMSG_CHANNEL_KICK                               = 0x010,
    CMSG_CHANNEL_BAN                                = 0x011,
    CMSG_CHANNEL_UNBAN                              = 0x012,
    CMSG_CHANNEL_ANNOUNCEMENTS                      = 0x013,
    CMSG_CHANNEL_MODERATE                           = 0x014,
    SMSG_QUEUE_POSITION                             = 0x015,
    NUM_MSG_TYPES                                   = 0x016,
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

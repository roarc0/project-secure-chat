#include "networking/opcode.h"

// Devono essere allineati con opcode.h
OpcodeHandler opcodeTable[NUM_MSG_TYPES] =
{ 
    { "MSG_NULL_ACTION",             STATUS_NEVER,       PROCESS_INPLACE,           &Session::Handle_NULL                           },
    { "CMSG_PING",                   STATUS_LOGGED,      PROCESS_INPLACE,           &Session::Handle_Ping                           },
    { "SMSG_PING",                   STATUS_LOGGED,      PROCESS_INPLACE,           &Session::Handle_ServerSide                     },
    { "SMSG_QUEUE_POSITION",         STATUS_LOGGED,      PROCESS_INPLACE,           &Session::Handle_ServerSide                     },
    { "SMSG_SYSTEM_MESSAGE",         STATUS_NEVER,       PROCESS_INPLACE,           &Session::Handle_ServerSide                     },
    { "CMSG_MESSAGE",                STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleMessage                         },
    { "SMSG_MESSAGE",                STATUS_NEVER,       PROCESS_INPLACE,           &Session::Handle_ServerSide                     },
    { "CMSG_WHISP",                  STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleWhisp                           },
    { "SMSG_WHISP",                  STATUS_NEVER,       PROCESS_INPLACE,           &Session::Handle_ServerSide                     },
    { "CMSG_CREATE_CHANNEL",         STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleCreateChannel                   },
    { "CMSG_JOIN_CHANNEL",           STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::HandleJoinChannel                     },
    { "CMSG_LEAVE_CHANNEL",          STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::HandleLeaveChannel                    },
    { "CMSG_CHANNEL_LIST",           STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleListChannel                     },  
    { "CMSG_CHANNEL_INFO",           STATUS_LOGGED,      PROCESS_INPLACE,           &Session::Handle_NULL                           },    
};

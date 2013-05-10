#include "networking/opcode.h"
#include "session.h"

// Devono essere allineati con opcode.h
OpcodeHandler opcodeTable[NUM_MSG_TYPES] =
{ 
    { "MSG_NULL_ACTION",             STATUS_NEVER,       PROCESS_INPLACE,           &Session::HandleNULL                           },
    { "CMSG_PING",                   STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandlePing                           },
    { "SMSG_PING",                   STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleServerSide                     },
    { "SMSG_QUEUE_POSITION",         STATUS_LOGGING,     PROCESS_INPLACE,           &Session::HandleServerSide                     },
    { "SMSG_SYSTEM_MESSAGE",         STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleServerSide                     },
    { "CMSG_MESSAGE",                STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleMessage                        },
    { "SMSG_MESSAGE",                STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleServerSide                     },
    { "CMSG_WHISP",                  STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleWhisp                          },
    { "SMSG_WHISP",                  STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleServerSide                     },
    { "CMSG_CREATE_CHANNEL",         STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleCreateChannel                  },
    { "CMSG_JOIN_CHANNEL",           STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::HandleJoinChannel                    },
    { "SMSG_JOIN_CHANNEL",           STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleServerSide                     },
    { "CMSG_LEAVE_CHANNEL",          STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::HandleLeaveChannel                   },
    { "SMSG_LEAVE_CHANNEL",          STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleServerSide                     },
    { "CMSG_CHANNEL_LIST",           STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleListChannel                    },
    { "CMSG_CHANNEL_INFO",           STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleNULL                           },
    { "CMSG_LOGIN",                  STATUS_LOGGING,     PROCESS_THREADUNSAFE,      &Session::HandleLogin                          },
    { "SMSG_LOGIN",                  STATUS_LOGGING,     PROCESS_INPLACE,           &Session::HandleServerSide                     }, 
};

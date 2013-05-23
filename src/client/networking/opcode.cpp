#include "networking/opcode.h"
#include "session.h"

// Devono essere allineati con opcode.h
OpcodeHandler opcodeTable[NUM_MSG_TYPES] =
{ 
    { "MSG_NULL_ACTION",             STATUS_NEVER,       PROCESS_INPLACE,           &Session::HandleNULL                           },
    { "CMSG_PING",                   STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleClientSide                     },
    { "SMSG_PING",                   STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandlePing                           },
    { "SMSG_QUEUE_POSITION",         STATUS_LOGGING,     PROCESS_INPLACE,           &Session::HandleQueuePos                       },
    { "SMSG_SYSTEM_MESSAGE",         STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleServerMessage                  },
    { "CMSG_MESSAGE",                STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleClientSide                     },
    { "SMSG_MESSAGE",                STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleMessage                        },
    { "CMSG_WHISP",                  STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleClientSide                     },
    { "SMSG_WHISP",                  STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleWhisp                          },
    { "CMSG_CREATE_CHANNEL",         STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleClientSide                     },
    { "CMSG_JOIN_CHANNEL",           STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleClientSide                     },
    { "SMSG_JOIN_CHANNEL",           STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleJoinChannel                    },
    { "CMSG_LEAVE_CHANNEL",          STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleClientSide                     },
    { "SMSG_LEAVE_CHANNEL",          STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleLeaveChannel                   },
    { "CMSG_CHANNEL_LIST",           STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleClientSide                     },  
    { "CMSG_CHANNEL_INFO",           STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleClientSide                     },   
    { "CMSG_LOGIN",                  STATUS_LOGGING,     PROCESS_INPLACE,           &Session::HandleClientSide                     },
    { "SMSG_LOGIN",                  STATUS_LOGGING,     PROCESS_INPLACE,           &Session::HandleLogin                          },
    { "CMSG_REFRESH_KEY",            STATUS_LOGGING,     PROCESS_INPLACE,           &Session::HandleClientSide                     },
    { "SMSG_REFRESH_KEY",            STATUS_LOGGING,     PROCESS_INPLACE,           &Session::HandleRefreshKey                     },
    { "CMSG_USERS_CHANNEL_LIST",     STATUS_LOGGING,     PROCESS_INPLACE,           &Session::HandleClientSide                     },
    { "SMSG_USERS_CHANNEL_LIST",     STATUS_LOGGING,     PROCESS_INPLACE,           &Session::HandleChannelUsersList               },  
};

#include "networking/opcode.h"

// Devono essere allineati con opcode.h
OpcodeHandler opcodeTable[NUM_MSG_TYPES] =
{ 
    { "MSG_NULL_ACTION",             STATUS_NEVER,       PROCESS_INPLACE,           &Session::Handle_NULL                           },
    { "CMSG_PING",                   STATUS_LOGGED,      PROCESS_INPLACE,           &Session::Handle_Ping                           },
    { "SMSG_PING",                   STATUS_LOGGED,      PROCESS_INPLACE,           &Session::Handle_ServerSide                     },
    { "CMSG_MESSAGE",                STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleMessage                         },
    { "SMSG_MESSAGE",                STATUS_NEVER,       PROCESS_INPLACE,           &Session::Handle_ServerSide                     },
    { "SMSG_SYSTEM_MESSAGE",         STATUS_NEVER,       PROCESS_INPLACE,           &Session::Handle_ServerSide                     },
    { "CMSG_JOIN_CHANNEL",           STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::HandleJoinChannel                     },
    { "CMSG_LEAVE_CHANNEL",          STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    { "SMSG_CHANNEL_NOTIFY",         STATUS_NEVER,       PROCESS_INPLACE,           &Session::Handle_ServerSide                     },
    { "CMSG_CHANNEL_LIST",           STATUS_LOGGED,      PROCESS_INPLACE,           &Session::Handle_NULL                           },
    { "SMSG_CHANNEL_LIST",           STATUS_NEVER,       PROCESS_INPLACE,           &Session::Handle_ServerSide                     },
    { "CMSG_CHANNEL_PASSWORD",       STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    { "CMSG_CHANNEL_SET_OWNER",      STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    { "CMSG_CHANNEL_OWNER",          STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    { "CMSG_CHANNEL_MODERATOR",      STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    { "CMSG_CHANNEL_UNMODERATOR",    STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    { "CMSG_CHANNEL_MUTE",           STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    { "CMSG_CHANNEL_UNMUTE",         STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    { "CMSG_CHANNEL_INVITE",         STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    { "CMSG_CHANNEL_KICK",           STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    { "CMSG_CHANNEL_BAN",            STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    { "CMSG_CHANNEL_UNBAN",          STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    { "CMSG_CHANNEL_ANNOUNCEMENTS",  STATUS_LOGGED,      PROCESS_INPLACE,           &Session::Handle_NULL                           },
    { "CMSG_CHANNEL_MODERATE",       STATUS_LOGGED,      PROCESS_INPLACE,           &Session::Handle_NULL                           },
    { "SMSG_QUEUE_POSITION",         STATUS_LOGGED,      PROCESS_INPLACE,           &Session::Handle_ServerSide                     },
};

#include "opcode.h"

OpcodeHandler opcodeTable[NUM_MSG_TYPES] =
{ 
    /*0x000*/ { "MSG_NULL_ACTION",             STATUS_NEVER,       PROCESS_INPLACE,           &Session::Handle_NULL                           },
    /*0x001*/ { "CMSG_MESSAGE",                STATUS_LOGGED,      PROCESS_INPLACE,           &Session::HandleMessage                         },
    /*0x002*/ { "SMSG_MESSAGE",                STATUS_NEVER,       PROCESS_INPLACE,           &Session::Handle_ServerSide                     },
    /*0x003*/ { "CMSG_JOIN_CHANNEL",           STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::HandleJoinChannel                     },
    /*0x004*/ { "CMSG_LEAVE_CHANNEL",          STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    /*0x005*/ { "SMSG_CHANNEL_NOTIFY",         STATUS_NEVER,       PROCESS_INPLACE,           &Session::Handle_ServerSide                     },
    /*0x006*/ { "CMSG_CHANNEL_LIST",           STATUS_LOGGED,      PROCESS_INPLACE,           &Session::Handle_NULL                           },
    /*0x007*/ { "SMSG_CHANNEL_LIST",           STATUS_NEVER,       PROCESS_INPLACE,           &Session::Handle_ServerSide                     },
    /*0x008*/ { "CMSG_CHANNEL_PASSWORD",       STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    /*0x009*/ { "CMSG_CHANNEL_SET_OWNER",      STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    /*0x00A*/ { "CMSG_CHANNEL_OWNER",          STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    /*0x00B*/ { "CMSG_CHANNEL_MODERATOR",      STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    /*0x00C*/ { "CMSG_CHANNEL_UNMODERATOR",    STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    /*0x00D*/ { "CMSG_CHANNEL_MUTE",           STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    /*0x00E*/ { "CMSG_CHANNEL_UNMUTE",         STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    /*0x00F*/ { "CMSG_CHANNEL_INVITE",         STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    /*0x010*/ { "CMSG_CHANNEL_KICK",           STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    /*0x011*/ { "CMSG_CHANNEL_BAN",            STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    /*0x012*/ { "CMSG_CHANNEL_UNBAN",          STATUS_LOGGED,      PROCESS_THREADUNSAFE,      &Session::Handle_NULL                           },
    /*0x013*/ { "CMSG_CHANNEL_ANNOUNCEMENTS",  STATUS_LOGGED,      PROCESS_INPLACE,           &Session::Handle_NULL                           },
    /*0x014*/ { "CMSG_CHANNEL_MODERATE",       STATUS_LOGGED,      PROCESS_INPLACE,           &Session::Handle_NULL                           },
    /*0x015*/ { "SMSG_QUEUE_POSITION",         STATUS_LOGGED,      PROCESS_INPLACE,           &Session::Handle_ServerSide                     },
};

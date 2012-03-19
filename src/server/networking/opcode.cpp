#include "opcode.h"

OpcodeHandler opcodeTable[NUM_MSG_TYPES] =
{
    /*0x000*/ { "MSG_NULL_ACTION",        STATUS_NEVER,     PROCESS_INPLACE,      &Session::Handle_NULL     },
    /*0x000*/ { "CMSG_MESSAGE",           STATUS_NEVER,     PROCESS_INPLACE,      &Session::HandleMessage   },
    /*0x000*/ { "SMSG_MESSAGE",           STATUS_NEVER,     PROCESS_INPLACE,      &Session::Handle_NULL     },
};

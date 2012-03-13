#include "opcode.h"

OpcodeHandler opcodeTable[NUM_MSG_TYPES] =
{
    /*0x000*/ { "MSG_NULL_ACTION", STATUS_NEVER, PROCESS_INPLACE, &Session::Handle_NULL },
}

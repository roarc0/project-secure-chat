#ifndef COMMAND_H
#define COMMAND_H

#include "common.h"

typedef enum cmd_type
{
    CMD_QUIT,
    CMD_INFO,
    CMD_CONNECT,
    CMD_PING,
    CMD_ACK,
    CMD_ABORT,
    CMD_SENDMSG,
    CMD_SENDPRIVMSG,
    CMD_JOIN,
    CMD_LEAVE,
    CMD_USERINFO
} cmd_type;

typedef bool(*handler)(char *, void *);

class command
{
    cmd_type    type;
    char        *name;
    bool        needparam;
    handler     h;

    //static cmd_type getType(char* name)
    //static char* getName(cmd_type type)
  public:
    command(char *, command *);
    command(cmd_type, command *);

    bool send(int, char *, unsigned int *);
    int  recv(char *, unsigned int *);
    char *forge_packet(int, const char *);
};

#endif

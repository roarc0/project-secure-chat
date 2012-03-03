#ifndef SERVER_CORE_H
#define SERVER_CORE_H

#include "networking/socket-server.h"
#include "execution/command-manager-server.h"
#include "session/execution-threads.h"
#include "session/network-threads.h"

void  server_core();

#endif

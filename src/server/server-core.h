#ifndef SERVER_CORE_H
#define SERVER_CORE_H

#include "../shared/common.h"
#include "../shared/networking/socket.h"
#include "../shared/execution/command-manager.h"
#include "session/session-manager.h"
#include "session/execution-threads.h"
#include "session/network-threads.h"
#include "execution/functions-server.h"
#include "database/database-manager.h"

void  start_server_core();

#endif

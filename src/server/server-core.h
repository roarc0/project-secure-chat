#ifndef SERVER_CORE_H
#define SERVER_CORE_H

#include "../shared/common.h"
#include "../shared/networking/socket.h"
#include "../shared/execution/command_manager.h"
#include "session/session-manager.h"
#include "server-functions.h"

void  start_server_core();

#endif

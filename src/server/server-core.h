#ifndef SERVER_CORE_H
#define SERVER_CORE_H

#include "socket-server.h"
#include "scheduling-engine.h"
#include "network-manager.h"
#include "session/session-manager.h"
#include "database/database-manager.h"

void  server_core();

#endif

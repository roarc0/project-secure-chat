#ifndef CORE_SERVER_H
#define CORE_SERVER_H

#include "../shared/common.h"
#include "../shared/networking/socket.h"
#include "../shared/threading/thread.h"

struct core_thread_params
{
    TCPSocket   *sock;
    //?
};

void start_server_core();
void* core_thread(void*);

#endif

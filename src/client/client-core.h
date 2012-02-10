#ifndef CORE_CLIENT_H
#define CORE_CLIENT_H

#include "../shared/common.h"
#include "../shared/networking/socket.h"
#include "../shared/threading/thread.h"
//#include "../shared/session/user-session.h"

bool  connect();
bool  disconnect();
void  handle_message(char*);

void* core_thread(void*);

#endif

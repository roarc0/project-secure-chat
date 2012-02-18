#ifndef CORE_CLIENT_H
#define CORE_CLIENT_H

#include "../shared/common.h"
#include "../shared/networking/socket.h"
#include "../shared/threading/thread.h"
#include "../shared/execution/command-manager.h"

#define c_core       client_core::get_instance()

#undef  SERVER

void* core_thread(void*);

class client_core
{
    bool connected;
    static client_core*    ccore_singleton;

          client_core();

  public:

    static client_core* get_instance()
    {
        if(!ccore_singleton)
            ccore_singleton = new client_core();
        return ccore_singleton;
    };
    
          ~client_core();

    bool  connect();
    bool  disconnect();
    void  handle_message(const char*);

    void  gui_update_status_bar(const char*);
    
    bool is_connected() const
    {
        return connected;
    }

    void set_connected(bool c)
    {
        connected = c;
    }

};

#endif

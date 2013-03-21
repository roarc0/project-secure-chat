#include "server-core.h"

#define N_NET_THREAD 2

void handle_session_manager_task(void *ptr)
{
    s_manager->AddTaskToServe(ptr);
}

void server_core()
{
    try
    {
        //db_manager->set_dbfilename(CFG_GET_STRING("db_filename"));
        //db_manager->init_db();

        s_manager;

        InitCommands();

        net_manager->Initialize(N_NET_THREAD); // Numero thread network
        s_sched_engine->Initialize(4); // Numero thread elaborazione

        SocketServer server;
        server.Init(CFG_GET_INT("server_port"));
        server.InitCallback(&handle_session_manager_task);

        INFO("debug", "* listening on port: %d\n", CFG_GET_INT("server_port"));
        server.Start();

        bool m_active = true;
        uint32 diff = 0;

        while(m_active)
        {
            try
            {
                // calcola diff
                s_manager->Update(diff);
            }
            catch(...)
            {
                INFO("debug", "default exception");
            }
        }
    }
    catch(...)
    {
        INFO("debug", "default init exception");
    }
}

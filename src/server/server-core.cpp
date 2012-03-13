#include "server-core.h"

void handle_session_manager_task(void *ptr)
{
    s_manager->addTaskToServe(ptr);
}

void server_core()
{
    try
    {
        db_manager->set_dbfilename(CFG_GET_STRING("db_filename"));
        db_manager->init_db();

        s_manager;
        s_sched_engine->Initialize(4); // Numero thread
        
        InitCommands();

        SocketServer server;
        server.init(CFG_GET_INT("server_port"));
        server.init_callback(&handle_session_manager_task);       

        INFO("debug", "* listening on port: %d\n", CFG_GET_INT("server_port"));

        network_threads net;
        execution_threads exec;
        
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
    catch(SocketException &e)
    {
        INFO("debug", "%s\n", e.what());
    }
    catch(...)
    {
        INFO("debug", "default init exception");
    }
}

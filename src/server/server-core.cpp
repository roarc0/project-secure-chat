#include "server-core.h"

void handle_session_manager_task(void *ptr)
{
    s_manager->addTaskToServe(ptr);
}

void server_core()
{
    try
    {
        whoami = "server";
        db_manager->set_dbfilename(CFG_GET_STRING("db_filename"));
        db_manager->init_db();

        s_manager;
        init_commands();

        SocketServer server;
        server.init(CFG_GET_INT("server_port"));
        server.init_callback(&handle_session_manager_task);       

        //UserSession *temp_session = NULL;
        INFO("debug", "* listening on port: %d\n", CFG_GET_INT("server_port"));

        network_threads net;
        execution_threads exec;

        //while(1)
        {
            try
            {
                //INFO("debug", "* client session created! %s:%d\n", 
                //     temp_sock->getForeignAddress().c_str(),
                //     temp_sock->getForeignPort());

                net.start_net_thread();
                exec.start_exec_thread();
            }
            catch(...)
            {
                INFO("debug", "default exception");
            }
            msleep(1000);
        }

        while(1);

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

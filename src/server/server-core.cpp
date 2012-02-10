#include "server-core.h"

void server_core()
{
    try
    {   
        db_manager->set_dbfilename(CFG_GET_STRING("db_filename"));
        db_manager->init_db();

        s_manager;
        init_commands();

        TCPServerSocket server(CFG_GET_INT("server_port"), false, 128);
        TCPSocket *temp_sock = NULL;
        UserSession *temp_session = NULL;
        INFO("debug", "* listening on port: %d\n", CFG_GET_INT("server_port"));

        network_threads net;
        execution_threads exec;

        while(1)
        {
            try
            {
                temp_sock = server.accept();
                temp_sock->setBlocking(false);
                s_manager->createSession(temp_sock);
                INFO("debug", "* new client session created!\n");

                net.start_net_thread();
                exec.start_exec_thread();
            }
            catch(SocketException &e)
            {
                INFO("debug", "[servercore] %s\n", e.what());
            }
            catch(...)
            {
                INFO("debug", "default exception");
            }
            usleep(50);
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

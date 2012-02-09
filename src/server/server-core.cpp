#include "server-core.h"

void init_commands() // va in un file commands-server.cpp/h che include functions
{
   //qui da aggiungere tutti i comandi con handler ecc
}

void start_server_core()
{
    try
    {
        TCPServerSocket server(CFG_GET_INT("server_port"), 128);
        TCPSocket *temp_sock = NULL;
        UserSession *temp_session = NULL;
        INFO("debug", "* listening on port: %d\n", CFG_GET_INT("server_port"));

        init_commands();

        db_manager->set_dbfilename(CFG_GET_STRING("db_filename"));
        db_manager->init_db();

        s_manager;

        network_threads net;
        execution_threads exec;


        while(1)
        {
            try
            {
                temp_sock = server.accept();
                temp_sock->setBlocking(false);
                s_manager->createSession(temp_sock);

                net.start_net_thread();
                exec.start_exec_thread();

                INFO("debug", "* new client session created!\n");
            }
            catch(SocketException &e)
            {
                INFO("debug", "%s\n", e.what());
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

#include "server-core.h"

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

        //UserSession *temp_session = NULL;
        INFO("debug", "* listening on port: %d\n", CFG_GET_INT("server_port"));

        network_threads net;
        execution_threads exec;

        while(1)
        {
            /*try
            {
                temp_sock = server.accept();
                s_manager->createSession(temp_sock);
                INFO("debug", "* client session created! %s:%d\n", 
                     temp_sock->getForeignAddress().c_str(),
                     temp_sock->getForeignPort());

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
            }*/
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

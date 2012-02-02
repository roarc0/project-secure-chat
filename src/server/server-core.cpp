#include "server-core.h"

void init_commands()
{
   //qui da aggiungere tutti i comandi con handler ecc
}

void start_server_core()
{
    TCPServerSocket server(CFG_GET_INT("server_port"), 128);
    TCPSocket *temp_sock = NULL;
    UserSession *temp_session = NULL;
    cout << "listening on port: " << CFG_GET_INT("server_port") << endl;

    init_commands();

    execution exec;
    exec.start_threads(c_manager, s_manager, 4);

    while(1)
    {
        temp_sock = server.accept();
        s_manager->createSession(temp_sock);
        cout << "client connected!" << endl;
    }
}

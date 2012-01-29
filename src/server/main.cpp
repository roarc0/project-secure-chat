#include <iostream>

#include "../shared/networks/socket.h"
#include "config-server.h"

using namespace std;

struct session_thread_params
{
    TCPSocket   *sock;
    //?
};

void *session_thread(void *arg)
{
    session_thread_params* t_param = (session_thread_params*) arg;
    
    if(!t_param)
        pthread_exit(NULL);

    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
    
    TCPSocket *sock = t_param->sock;
    
    while(1); // gestore comunicazione

    if (t_param)
        delete t_param;

    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    welcome();

    init_config("pschat-server.conf");

    TCPServerSocket server(CFG_GET_INT("port"), 128);
    TCPSocket *temp = NULL;

    int ret;

    while(1)
    {    
        pthread_t      tid;
        pthread_attr_t tattr;

        temp = server.accept();
        cout << "connection!" << endl;
        
        ret = pthread_attr_init(&tattr);
        ret = pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);

        session_thread_params *t_params = new session_thread_params;
        t_params->sock = temp;

        if (ret = pthread_create(&tid, &tattr, &session_thread, (void*)t_params))
        {
            perror("pthread_create");
            delete t_params;
        }

        //tids.push_back(tid);
        pthread_attr_destroy(&tattr);
    }

    return 0;
}

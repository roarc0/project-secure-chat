#include "socket-server.h"

int main(int argc, char **argv)
{
    SocketServer srv;

    int status;
    pthread_t *threads = NULL;

    srv.init(7777);

    threads = (pthread_t *) malloc(sizeof(*threads) * 6);

    //for (int i = 0, cpu = 0; i < 6; ++i)
        pthread_create(&threads[0], NULL, epoll_thread, (void*)&srv);

    cout << "running..." << endl;
    while(1);

    free(threads);

    return 0;
}


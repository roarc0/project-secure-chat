#include "../socket-client.h"

int main()
{
    SocketClient cli(SOCK_STREAM,0);

    cli.connect("127.0.0.1",7777);

    while(1)
    {
        cli.send("ciao server\n", 12);
        sleep(4);
    }
    
    return 0;
}

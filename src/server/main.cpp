#include <iostream>

#include "../shared/common.h"
#include "../shared/networks/socket.h"

using namespace std;

int main(int argc, char** argv)
{
    TCPServerSocket server(555, 128);
    return 0;
}

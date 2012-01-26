#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    TCPClientSocket server(6666, 128);
    server.accept();
    return 0;
}

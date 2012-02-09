#include "server-core.h"
#include "server-config.h"

using namespace std;

int main(int argc, char** argv)
{
    welcome();

    init_config("pschat-server.conf");
    server_core();

    return 0;
}

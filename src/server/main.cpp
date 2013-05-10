#include "server-core.h"
#include "server-config.h"

using namespace std;

int main(int argc, char** argv)
{
    welcome();

    Init(argc, argv);
    TestRsa();
    ServerCore();

    return 0;
}

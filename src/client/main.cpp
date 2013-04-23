#include "client-core.h"
#include "client-config.h"
#include "gui.h"

using namespace std;

int main(int argc, char** argv)
{
    welcome();

    init_config("../etc/psc-client.conf");

    c_core;
    main_gui(argc, argv);

    return 0;
}

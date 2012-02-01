#include "client-core.h"
#include "client-config.h"
#include "gui.h"

using namespace std;

int main(int argc, char** argv)
{
    welcome();

    init_config("pschat-client.conf");
    start_thread(&core_thread, (void*)NULL);

    main_gui(argc,argv);

    return 0;
}

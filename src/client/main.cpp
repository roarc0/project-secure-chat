#include "client-core.h"
#include "client-config.h"
#include "gui.h"

using namespace std;

int main(int argc, char** argv)
{
    welcome();

    Init(argc, argv);

    //c_core->TestRsa(); /* test on connection */
    main_gui(argc, argv);

    return 0;
}

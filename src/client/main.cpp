#include "client-core.h"
#include "client-config.h"
#include "gui.h"

using namespace std;

int main(int argc, char** argv)
{
    welcome();

    Init(argc, argv);

    main_gui(argc, argv);

    return 0;
}

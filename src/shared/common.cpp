#include "common.h"

logger* logger::log_singleton = NULL;
config* config::cfg_singleton = NULL;
 
void welcome()
{
    cout << "Project Secure Chat v" << "PACKAGE_VERSION" << " (waiting for a better name)" << endl;
    cout << "Authors: Alessandro Rosetti - Daniele Lazzarini - Alessandro Furlanetto" << endl;
    cout << "Software is under license: GPLv3" << endl;
    cout << endl;
}

int exec_command(const char *fmt, ...)
{
    char buffer[256]; // FIXME ...
    int ret;

    va_list ap;
    va_start(ap, fmt);
    ret = vsnprintf(buffer, 256, fmt, ap);
    va_end(ap);

    return system(buffer);
}

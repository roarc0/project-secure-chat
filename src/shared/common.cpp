#include "common.h"

logger *logger::l_singleton = NULL;

void welcome()
{
    cout << PACKAGE_STRING << ": project secure chat ";
    cout << "authors: Alessandro Rosetti - Daniele Lazzarini - Alessandro Furlanetto" << endl;
    cout << "license: GPLv3" << endl;
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

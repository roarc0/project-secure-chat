#include "common.h"

void welcome()
{
    cout << "Project Secure Chat v" << _REVISION << " " << endl;
    cout << "Authors: Alessandro Rosetti - Daniele Lazzarini - _(Alessandro Furlanetto)_" << endl;
    cout << "Software is under license: GPLv3" << endl;
    cout << endl;
}

int exec_command(const char *fmt, ...)
{
    char *buffer = NULL;
    //int ret;

    va_list ap;
    va_start(ap, fmt);
    if (vasprintf(&buffer, fmt, ap) < 0)
        return -1;
    va_end(ap);

    if (buffer)
        delete[] buffer;

    return system(buffer);
}

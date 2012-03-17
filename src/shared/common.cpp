#include "common.h"

void welcome()
{
    cout << "Project Secure Chat v" << "_REVISION" << " (waiting for a better name)" << endl;
    cout << "Authors: Alessandro Rosetti - Daniele Lazzarini - Alessandro Furlanetto" << endl;
    cout << "Software is under license: GPLv3" << endl;
    cout << endl;
}

int exec_command(const char *fmt, ...)
{
    char *buffer = NULL;
    int ret;

    va_list ap;
    va_start(ap, fmt);
    if (vasprintf(&buffer, fmt, ap) < 0)
        return -1;
    va_end(ap);

    if (buffer)
        delete[] buffer;

    return system(buffer);
}

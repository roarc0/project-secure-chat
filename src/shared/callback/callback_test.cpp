#include <cstdio>
#include <cstdlib>
#include <string>
#include "callback.h"

using namespace std;

int my_event_cb(int)
{
    printf("callback int return, int param test!\n");
    return 0;
}

string my_event_cb_s(int)
{
    printf("callback string return, int param test!\n");
    return "ciao";
}

int main()
{
    event_callback<int,int> cb(my_event_cb);

    printf("ret: %d\n", cb(1));

    event_callback<string, int> cb_s(my_event_cb_s);

    printf("ret: %s\n", cb_s(1).c_str());

    return 0;
}

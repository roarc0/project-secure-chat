#include "functions.h"

bool Dummy(void* params)
{
    if(IS_SERVER) //orribilissimo
       INFO("debug", "server received dummy message!\n");
    else
       INFO("debug", "client received dummy message!\n");

   return true;
}

bool Ping(void* params)
{
   handler_params* hparams = (handler_params*) params;

   return true;
}

bool Pong(void* params)
{

   return true;
}

#include "functions.h"

bool dummy(void* params)
{
    if(IS_SERVER) //orribilissimo
       INFO("debug", "server received dummy message!\n");
    else
       INFO("debug", "client received dummy message!\n");

   return true;
}

bool ping(void* params)
{
   handler_params* hparams = (handler_params*) params;

   return true;
}

bool pong(void* params)
{

   return true;
}

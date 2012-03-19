#include "functions.h"

bool Dummy(void* params)
{
   INFO("debug", "received dummy message!\n");

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

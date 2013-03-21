#include "functions-server.h"
/*
bool SendMessage(void* params)
{
    handler_params* hpar = (handler_params*)params;
    std::list<uint32> id_list;
    int sender_id = 0; //= s_manager->GetUsersessionId(hpar->usession);

    if (sender_id == 0)
        return false;

    Packet* pack = new Packet(0);
    //pack->m_data = hpar->params;

    INFO("debug", "sender id is %d\n", sender_id);

    s_manager->GetIdList(&id_list);
    std::list<uint32>::iterator itr;
    for(itr = id_list.begin(); itr != id_list.end() ; itr++)
        if (*(itr) != sender_id)
        {
            INFO("debug", "sending msg to id %d\n", *(itr));
            //s_manager->SendPacketTo(*(itr), pack);
        }
    return true;
}*/

bool UpdateUserList(void* params)
{

    return true;
}

#include "server-core.h"

#define N_NET_THREAD 2
#define N_EXEC_THREAD 4

/*void handle_session_manager_task(void *ptr)
{
    s_manager->AddTaskToServe(ptr);
}*/

void server_core()
{
    try
    {
        //db_manager->set_dbfilename(CFG_GET_STRING("db_filename"));
        //db_manager->init_db();

        s_manager;
        net_manager;

        net_manager->Initialize(N_NET_THREAD); // Numero thread network
        s_sched_engine->Initialize(N_EXEC_THREAD); // Numero thread elaborazione

        bool m_active = true;
        uint32 diff = 0;

        while(m_active)
        {
            try
            {
                msleep(100);
                // calcola diff
                diff = 100;
                s_manager->Update(diff);
            }
            catch(...)
            {
                INFO("debug", "default exception");
            }
        }
    }
    catch(...)
    {
        INFO("debug", "default init exception");
    }
}

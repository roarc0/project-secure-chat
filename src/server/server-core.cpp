#include "server-core.h"

void server_core()
{
    try
    {
        //db_manager->set_dbfilename(CFG_GET_STRING("db_filename"));
        //db_manager->init_db();

        s_manager;
        net_manager;

        net_manager->Initialize(CFG_GET_INT("ThreadNet")); // Numero thread network
        s_sched_engine->Initialize(CFG_GET_INT("ThreadExec")); // Numero thread elaborazione

        bool m_active = true;
        double diff = 0;
        struct timer tm;

        while(m_active)
        {
            time_start(&tm);
            try
            {
                msleep(100);

                time_stop(&tm);
                diff = time_diff(&tm);
                if (diff == 0)
                    diff = 1;

                s_manager->Update((uint32)diff);
            }
            catch(...)
            {
                INFO("debug", "default exception");
            }
        }
    }
    catch(...)
    {
        INFO("debug", "SERVER_CORE: default init exception");
    }
}

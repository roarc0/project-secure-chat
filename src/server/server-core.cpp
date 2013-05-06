#include "server-core.h"

void server_core()
{
    try
    {
        db_manager->SetFilename(CFG_GET_STRING("db_filename"));
        db_manager->OpenDb();

        s_manager;
        net_manager;

        net_manager->Initialize(CFG_GET_INT("ThreadNet")); // Numero thread network
        s_sched_engine->Initialize(CFG_GET_INT("ThreadExec")); // Numero thread elaborazione

        bool m_active = true;
        uint32 diff = 0;
        struct timeval t1, t2;

        gettimeofday(&t1, NULL);

        while(m_active)
        {
            try
            {
                msleep(100);

                // calcola diff
                gettimeofday(&t2, NULL);
                diff = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec)/1000;
                if (diff == 0)
                    diff = 1;
                t1 = t2;
 
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
        INFO("debug", "SERVER_CORE: default init exception");
    }
}

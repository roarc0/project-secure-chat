#include "../../shared/session/user-session.h"
#include "../../shared/threading/mutex.h"

class Session
{
    public:
        Session(UserSession* pUser)
        {
            pthread_mutex_init(&mutex_net, NULL);
            pthread_mutex_init(&mutex_exec, NULL);
            m_pUser = pUser;
            m_pUser->SetSession(this);
            m_active = 1;
        }

        ~Session()
        {
            pthread_mutex_destroy(&mutex_net);
            pthread_mutex_destroy(&mutex_exec);
        }

        // TODO Inserire tempo di creazione della session per controllo di pacchetti precedenti
        void SetSession(UserSession* pUser, uint32 id) { m_pUser = pUser; m_active = 1; m_pUser->SetId(id); m_pUser->SetSession(this);}
        UserSession* GetUserSession() { return m_pUser; }

        bool IsActive() { return m_active == 1 ? true : false; }
        bool IsFree() { return m_active == 0 ? true : false; }
        bool IsToDelete() { return m_active == -1 ? true : false; }
        void Free() 
        {
            delete m_pUser;
            m_pUser = NULL;
            m_active = 0;
            releaselock_exec();
            releaselock_net();
        }

        void ToDelete()
        {
            m_active = -1;
        }

        void  releaselock_net() { pthread_mutex_unlock(&mutex_net); }
        // Non bloccante
        int getlock_net()
        {
            if (IsToDelete() || IsFree())
                return -1;
            if (pthread_mutex_trylock (&mutex_exec) != 0)
                return 0;
            else
                return 1;
            
        }        

        void  releaselock_exec() { pthread_mutex_unlock(&mutex_exec); }
        // Non bloccante
        bool getlock_exec()
        {
            if (!IsActive())
                return  false;
            if (pthread_mutex_trylock (&mutex_net) != 0)
                return  false;
            else
                return true;
        }

        bool TryDelete()
        {
            if (IsToDelete())
                if (pthread_mutex_trylock (&mutex_exec) == 0)           // Cerco di ottenere il mutex di exec
                    if (pthread_mutex_trylock (&mutex_net) == 0)        // Cerco di ottenere il mutex di net
                    {
                        Free();                                         // Cancello
                        return true;
                    }
                    else
                    {
                        releaselock_exec();                             // Non ho ottenuto il mutex di net, rilascio quello di exec
                        return false;
                    }
            return false;
        }
        
        Mutex mutex_session;        

    private:

        pthread_mutex_t    mutex_net;
        pthread_mutex_t    mutex_exec;

        int m_active;
        UserSession* m_pUser;
};

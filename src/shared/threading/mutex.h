#ifndef _MUTEX_H
#define _MUTEX_H

#include <pthread.h>

class Lock;
class TryLock;

class Mutex
{
    friend class Lock;
    friend class TryLock;

    public:
        Mutex () { pthread_mutex_init(&_mutex, NULL); }
        ~Mutex () { pthread_mutex_destroy(&_mutex); }

        void Acquire ()
        {
            pthread_mutex_lock(&_mutex);
        }

        bool TryAcquire ()
        {
            return (pthread_mutex_trylock (&mutex_exec) == 0);
        }

        void Release ()
        {
            pthread_mutex_unlock(&_mutex);
        }

    private:
        pthread_mutex_t _mutex;
};

#endif 

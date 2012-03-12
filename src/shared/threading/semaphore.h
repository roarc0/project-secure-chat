#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <pthread.h>
#include "mutex.h"

class Semaphore
{
    public:
     	Semaphore(Mutex& mutex)
        {
            _mutex = &mutex;
        }
        virtual ~Semaphore()
        {

        }
        bool wait()
        {
            pthread_cond_wait(&_cond, &(_mutex->_mutex));
        }
        void signal()
        {
            pthread_cond_signal(&_cond); 
        }
        void broadcast()
        {
            int pthread_cond_broadcast(&_cond); 
        }
    private:
        pthread_cond_t _cond;
        Mutex* _mutex;
};

#endif 

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
        bool Wait()
        {
            pthread_cond_wait(&_cond, &(_mutex->_mutex));
            return true;
        }
        void Signal()
        {
            pthread_cond_signal(&_cond); 
        }
        void Broadcast()
        {
            pthread_cond_broadcast(&_cond); 
        }
    private:
        pthread_cond_t _cond;
        Mutex* _mutex;
};

#endif 

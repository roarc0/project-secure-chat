#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <pthread.h>
#include "mutex.h"
#include "lock.h"

class Semaphore
{
    public:
     	Semaphore(Mutex& mutex, int val = 0)
        {
            _mutex = &mutex;
            pthread_cond_init(&_cond, NULL);
            value = val;
        }
        virtual ~Semaphore()
        {
            pthread_cond_destroy(&_cond);
        }
        bool Wait()
        {
            Lock locked(*_mutex);
            while (value <= 0)
            {
                pthread_cond_wait(&_cond, &(_mutex->_mutex));
            }
            value--;
            return true;
        }
        void Signal()
        {
            (*_mutex).Acquire();
            value++;
            (*_mutex).Release();
            pthread_cond_signal(&_cond);
        }
        /*void Broadcast()
        {
            pthread_cond_broadcast(&_cond); 
        }*/
    private:
        int value;
        pthread_cond_t _cond;
        Mutex* _mutex;
};

#endif 

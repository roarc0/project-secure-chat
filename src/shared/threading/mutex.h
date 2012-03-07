#if !defined _MUTEX_H_
#define _MUTEX_H_

#include <pthread.h>

class Mutex
{
    friend class Lock;
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

class Lock
{
    public:
        // Acquire the state of the semaphore
        Lock ( Mutex & mutex )
            : _mutex(mutex)
        {
            _mutex.Acquire();
        }
        // Release the state of the semaphore
        ~Lock ()
        {
            _mutex.Release();
        }
    private:
        Mutex & _mutex;
};

class TryLock
{
    public:
        // Acquire the state of the semaphore
        Lock ( Mutex & mutex , bool & locked)
            : _mutex(mutex)
        {
            _locked = locked = _mutex.TryAcquire();
        }
        // Release the state of the semaphore
        ~Lock ()
        {
            if (_locked)
                _mutex.Release();
        }
    private:
        Mutex & _mutex;
        bool _locked;
};

#endif 

#if !defined _MUTEX_H_
#define _MUTEX_H_

#include <pthread>

class Guard
{
    public:
        // Acquire the state of the semaphore
        Guard ( Mutex & mutex )
            : _mutex(mutex)
        {
            _mutex.Acquire();
        }
        // Release the state of the semaphore
        ~Guard ()
        {
            _mutex.Release();
        }
    private:
        Mutex & _mutex;
};

class Mutex
{
    friend class Guard;
    public:
        Mutex () { pthread_mutex_init(&_mutex, NULL); }
        ~Mutex () { pthread_mutex_destroy(&_mutex); }
        void Acquire ()
        {
            pthread_mutex_lock(&_mutex);
        }
        void Release ()
        {
            pthread_mutex_unlock(&_mutex);
        }
    private:
        pthread_mutex_t _mutex;
};

#endif 

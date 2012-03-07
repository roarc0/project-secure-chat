#ifndef _LOCK_H
#define _LOCK_H

#include "mutex.h"

class Lock
{
    public:
        Lock ( Mutex & mutex )
            : _mutex(mutex)
        {
            _mutex.Acquire();
        }

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
        Lock ( Mutex & mutex , bool & locked)
            : _mutex(mutex)
        {
            _locked = locked = _mutex.TryAcquire();
        }

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

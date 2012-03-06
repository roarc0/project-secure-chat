#if !defined _SINGLETON_H_
#define _SINGLETON_H_

#include "../threading/mutex.h"

class Singleton
{
    public:
        static Singleton *GetInstance( void )
        {
            Lock guard(s_mutex);
            if (!instance_)
                instance_ = new Singleton;
            return instance_;
        }
        static void DeleteInstance( void )
        {
            Lock guard(s_mutex);
            if (instance_)
                delete instance_;
            instance_ = NULL;
        }
    private:
        static Singleton *instance_;
        Singleton() {};
        ~Singleton() {};
        Singleton(const Singleton&);

        static Mutex s_mutex;
};

#endif 

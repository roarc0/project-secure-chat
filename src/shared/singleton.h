#if !defined _SINGLETON_H_
#define _SINGLETON_H_

class Singleton
{
    public:
        static Singleton *GetInstance( void )
        {
            if (!instance_)
                instance_ = new Singleton;
            return instance_;
        }
        static void DeleteInstance( void )
        {
            if (instance_)
                delete instance_;
            instance_ = NULL;
        }
    private:
        static Singleton *instance_;
        Singleton() {};
        ~Singleton() {};
        Singleton(const Singleton&);
};

#endif 

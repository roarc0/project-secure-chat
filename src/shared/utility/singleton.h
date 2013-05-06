#ifndef _SINGLETON_H_
#define _SINGLETON_H_

template <class T>
class Singleton
{
    public:
        static T* GetInstance( void )
        {
            if (!instance_)
                instance_ = new T;

            return (static_cast<T*> (instance_));
        }
        static void DeleteInstance( void )
        {
            if (instance_)
            {
                delete instance_;
                instance_ = NULL;
            }
        }
    protected:   
        Singleton() {};
        ~Singleton() {};
        Singleton(const Singleton&);
    private:
        static T* instance_;        
};

template <typename T>
T *Singleton<T>::instance_ = NULL;

#endif 

#if !defined _SINGLETON_H_
#define _SINGLETON_H_

#include "../threading/mutex.h"

class Singleton
{
    public:
        //method to get Instance of class
        static Singleton *getInstance( void )
        {
            Lock guard(_mutex);
            //Note that the class is only created when this method is called first time
            if (!instance_)
                instance_ = new Singleton;
            return instance_;
        }
        //method to delete Instance of class
        static void deleteInstance( void )
        {
            Lock guard(_mutex);
            if(instance_)
                delete instance_;
            instance_ = NULL; //important as this can create dead reference problems
        }
    private:
        //variable to store the instance of singleton
        static Singleton *instance_;
        //default constructor should be private to prevent instantiation
        Singleton() {};
        //destructor should be made private so no one can delete this accidently
        ~Singleton() {};
        //We also need to prevent copy being created of the object
        Singleton(const Singleton&);

        Mutex _mutex;
};

#endif 

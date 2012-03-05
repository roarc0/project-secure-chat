#ifndef MT_QUEUE_H
#define MT_QUEUE_H

#include <list>
#include <pthread.h>

template <typename T>
class mt_queue
{
    public:
        mt_queue();
        ~mt_queue();

        T front_and_pop();
        void push(T);        
        void erase(typename std::list<T>::iterator);

    protected:
        void  get_lock() { pthread_mutex_lock(&m_mut); }
        void  release_lock() { pthread_mutex_unlock(&m_mut); }

        std::list<T>       m_list;
        pthread_mutex_t    m_mut;     

};

template <typename T>
mt_queue<T>::mt_queue()
{
    pthread_mutex_init(&m_mut, NULL);
}

template <typename T>
mt_queue<T>::~mt_queue()
{
    pthread_mutex_destroy(&m_mut);
}

template <typename T>
T mt_queue<T>::front_and_pop()
{
    get_lock();
    T temp = m_list.front();
    m_list.pop_front();
    release_lock();
    return temp;
}

template <typename T>
void mt_queue<T>::push(T a)
{
    get_lock();
    m_list.push_back(a);
    release_lock();
}

template <typename T>
void mt_queue<T>::erase(typename std::list<T>::iterator it)
{
    get_lock();
    m_list.erase(it);
    release_lock();
}

#endif

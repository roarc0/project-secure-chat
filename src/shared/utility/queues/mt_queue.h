#ifndef MT_QUEUE_H
#define MT_QUEUE_H

#include <list>
#include "../../threading/lock.h"

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
        void push_l(T);        
        void erase_l(typename std::list<T>::iterator);

        std::list<T>   _list;
        Mutex          _mutex;     

};

template <typename T>
mt_queue<T>::mt_queue()
{

}

template <typename T>
mt_queue<T>::~mt_queue()
{

}

template <typename T>
T mt_queue<T>::front_and_pop()
{
    Lock guard(_mutex);
    T temp = _list.front();
    _list.pop_front();
    return temp;
}

template <typename T>
void mt_queue<T>::push(T a)
{
    Lock guard(_mutex);
    _list.push_back(a);
}

template <typename T>
void mt_queue<T>::erase(typename std::list<T>::iterator it)
{
    Lock guard(_mutex);
    _list.erase(it);
}

template <typename T>
void mt_queue<T>::push_l(T a)
{
    _list.push_back(a);
}

template <typename T>
void mt_queue<T>::erase_l(typename std::list<T>::iterator it)
{
    _list.erase(it);
}

#endif

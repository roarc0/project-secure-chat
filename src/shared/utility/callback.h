#ifndef _CALLBACK_H_
#define _CALLBACK_H_

#include <cstdlib>

template < typename ret_type, typename arg_type >
class EventCallback
{
  public:
    typedef ret_type (*callback_t)(arg_type arg);
  
  private:  
    callback_t cb;

  public:
    EventCallback();
    EventCallback(callback_t);
    ~EventCallback();

    void RegisterCb(callback_t);

    ret_type operator()(arg_type);
    ret_type operator()();

    ret_type ExecCb(arg_type);
    ret_type ExecCb();
};

// se non metto le definizioni dei template nel file h il compilatore smatta...
template < typename ret_type, typename arg_type >
EventCallback<ret_type, arg_type>::EventCallback()
{
    this->cb = NULL;
}

template < typename ret_type, typename arg_type >
EventCallback<ret_type, arg_type>::EventCallback(callback_t cb)
{
    this->cb = cb;
}

template < typename ret_type, typename arg_type >
EventCallback<ret_type, arg_type>::~EventCallback()
{

}

template < typename ret_type, typename arg_type >
void EventCallback<ret_type, arg_type>::RegisterCb(callback_t cb)
{
    this->cb = cb;
}

template < typename ret_type, typename arg_type >
ret_type EventCallback<ret_type, arg_type>::operator()(arg_type arg)
{
    if(!cb)
        return (ret_type)0;

    return (*cb)(arg);
}

template < typename ret_type, typename arg_type >
ret_type EventCallback<ret_type, arg_type>::operator()()
{
    if(!cb)
        return (ret_type)0;

    return (*cb)(NULL);
}

template < typename ret_type, typename arg_type >
ret_type EventCallback<ret_type, arg_type>::ExecCb(arg_type arg)
{
    return *this(arg);
}

template < typename ret_type, typename arg_type >
ret_type EventCallback<ret_type, arg_type>::ExecCb()
{
    return *this(NULL);
}
#endif

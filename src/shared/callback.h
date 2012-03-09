#ifndef __CALLBACK_
#define __CALLBACK_

#include <cstdlib>

template < typename ret_type, typename arg_type >
class event_callback
{
  public:
    typedef ret_type (*callback_t)(arg_type arg);
  
  private:  
    callback_t cb;

  public:
    event_callback();
    event_callback(callback_t);
    ~event_callback();

    void register_cb(callback_t);

    ret_type operator()(arg_type);
    ret_type operator()();

    ret_type exec_cb(arg_type);
    ret_type exec_cb();
};

// se non metto le definizioni dei template nel file h il compilatore smatta...
template < typename ret_type, typename arg_type >
event_callback<ret_type, arg_type>::event_callback()
{
    this->cb = NULL;
}

template < typename ret_type, typename arg_type >
event_callback<ret_type, arg_type>::event_callback(callback_t cb)
{
    this->cb = cb;
}

template < typename ret_type, typename arg_type >
event_callback<ret_type, arg_type>::~event_callback()
{

}

template < typename ret_type, typename arg_type >
void event_callback<ret_type, arg_type>::register_cb(callback_t cb)
{
    this->cb = cb;
}

template < typename ret_type, typename arg_type >
ret_type event_callback<ret_type, arg_type>::operator()(arg_type arg)
{
    if(!cb)
        return (ret_type)0;

    return (*cb)(arg);
}

template < typename ret_type, typename arg_type >
ret_type event_callback<ret_type, arg_type>::operator()()
{
    if(!cb)
        return (ret_type)0;

    return (*cb)(NULL);
}

template < typename ret_type, typename arg_type >
ret_type event_callback<ret_type, arg_type>::exec_cb(arg_type arg)
{
    return *this(arg);
}

template < typename ret_type, typename arg_type >
ret_type event_callback<ret_type, arg_type>::exec_cb()
{
    return *this(NULL);
}
#endif

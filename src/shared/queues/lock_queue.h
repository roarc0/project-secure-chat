#ifndef LOCKEDQUEUE_H
#define LOCKEDQUEUE_H

#include <deque>
#include "../threading/mutex.h"

template <class T, typename StorageType=std::deque<T> >
class LockedQueue
{
    //! Lock access to the queue.
    Mutex  _lock;

    //! Storage backing the queue.
    StorageType _queue;

    //! Cancellation flag.
    volatile bool _canceled;

    public:

        //! Create a LockedQueue.
        LockedQueue(): _canceled(false)
        {
        }

        //! Destroy a LockedQueue.
        virtual ~LockedQueue()
        {
        }

        //! Adds an item to the queue.
        void add(const T& item)
        {
            lock();

            _queue.push_back(item);

            unlock();
        }

        //! Gets the next result in the queue, if any.
        bool next(T& result)
        {
            Lock guard(_lock);

            if (_queue.empty())
                return false;

            result = _queue.front();
            _queue.pop_front();

            return true;
        }

        template<class Checker>
        bool next(T& result, Checker& check)
        {
            Lock guard(_lock);

            if (_queue.empty())
                return false;

            result = _queue.front();
            if (!check.Process(result))
                return false;

            _queue.pop_front();
            return true;
        }

        bool next(T& result)
        {
            Lock guard(_lock);

            if (_queue.empty())
                return false;

            result = _queue.front();
            _queue.pop_front();
            return true;
        }

        T& peek(bool autoUnlock = true)
        {
            lock();

            T& result = _queue.front();

            if (autoUnlock)
                unlock();

            return result;
        }

        //! Cancels the queue.
        void cancel()
        {
            lock();

            _canceled = true;

            unlock();
        }

        //! Checks if the queue is cancelled.
        bool cancelled()
        {
            Lock guard(_lock);
            return _canceled;
        }

        //! Locks the queue for access.
        void lock()
        {
            this->_lock.acquire();
        }

        //! Unlocks the queue.
        void unlock()
        {
            this->_lock.release();
        }

        ///! Calls pop_front of the queue
        void pop_front()
        {
            Lock guard(_lock);
            _queue.pop_front();
        }

        ///! Checks if we're empty or not with locks held
        bool empty()
        {
            Lock guard(_lock);
            return _queue.empty();
        }

        int size()
        {
            Lock guard(_lock);
            return _queue.size();
        }
};

#endif

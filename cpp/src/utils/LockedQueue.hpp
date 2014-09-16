#ifndef LOCKED_QUEUE_H
#define LOCKED_QUEUE_H

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/xtime.hpp>
#include <iostream>
#include <deque>
#include "Logger.h"

// This class is a queue for communication between two threads. It
// provides thread safe push and pop. Popping an empty queue results
// in a wait until the queue is filled.
//
// Parameters:
// maxSize: If the queue, after push, exceeds this size the process sleeps for
// 2^(size-maxSize) mMaxSizeSleepSecs seconds. 

template<class T>
class LockedQueue{

public:
    LockedQueue()
        : mMaxSize(2), mMaxSizeSleepSecs(0)
        {}
    LockedQueue(size_t maxSize, size_t maxSizeSleepSecs)
        : mMaxSize(maxSize), mMaxSizeSleepSecs(maxSizeSleepSecs)
        {}
    ~LockedQueue();
    void push(T* t);
    void push(const T& t);
    void pushToFront(T* t);
    void pushToFront(const T& t);
    T* front();
    T* pop();
    T popNCopy();
    size_t size() const;
    bool locked() const;
    
private:
    void pauseIfFull() const;

private:
    const size_t            mMaxSize;
    const size_t            mMaxSizeSleepSecs;
    std::deque<T*>          mQueue;
    mutable boost::mutex    mMutex;
    boost::condition        mCondition;
};


template<class T>
LockedQueue<T>::~LockedQueue()
{
    boost::mutex::scoped_lock lock(mMutex);
    while(mQueue.size()){
        if (mQueue.front() != 0)
            delete mQueue.front();
        mQueue.pop_front();
    }
}

template<class T>
size_t 
LockedQueue<T>::size() const
{
    boost::mutex::scoped_lock lock(mMutex);
    return mQueue.size();
}

template<class T>
void 
LockedQueue<T>::pauseIfFull() const
{
    if (mQueue.size() >= mMaxSize){
        unsigned int sleepSecs = (unsigned int)
            round((exp2(mQueue.size() - mMaxSize) * mMaxSizeSleepSecs));
        LOG_DEBUG << "Queue size " << mQueue.size()
                  << " (max " << mMaxSize 
                  << ") sleeping for " << sleepSecs << " seconds";
        
        boost::xtime xt;
        boost::xtime_get(&xt, 1); 
// the second parameter is a placeholder for boost::TIME_UTC_ in boost >= 50 and boost::TIME_UTC in boost < 50. Can be set to boost::TIME_UTC_ once everyone is on boost >= 50


        xt.sec += sleepSecs;
        boost::thread::sleep(xt);
    }
}

template<class T>
void 
LockedQueue<T>::push(T* t)
{
    {
        boost::mutex::scoped_lock lock(mMutex);
        mQueue.push_back(t);
        mCondition.notify_all();
    }
    pauseIfFull();
}

template<class T>
void 
LockedQueue<T>::pushToFront(T* t)
{
    {
        boost::mutex::scoped_lock lock(mMutex);
        mQueue.push_front(t);
        mCondition.notify_all();
    }
    pauseIfFull();
}

template<class T>
void 
LockedQueue<T>::push(const T& t)
{
    T* tp = new T(t);
    push(tp);
}

template<class T>
void 
LockedQueue<T>::pushToFront(const T& t)
{
    T* tp = new T(t);
    pushToFront(tp);
}

template<class T>
T*
LockedQueue<T>::pop()
{
    boost::mutex::scoped_lock lock(mMutex);
    while (mQueue.empty())
        mCondition.wait(lock);
    T* t = mQueue.front();
    mQueue.pop_front();
    return t;
}

template<class T>
T
LockedQueue<T>::popNCopy()
{
    T* tp = pop();
    T t(*tp);
    delete tp;
    return t;
}

template<class T>
T*
LockedQueue<T>::front()
{
    boost::mutex::scoped_lock lock(mMutex);
    while (mQueue.empty())
        mCondition.wait(lock);
    T* t = mQueue.front();
    return t;
}

#endif //LOCKED_QUEUE_H

#ifndef THREAD_H
#define THREAD_H

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <string>

class Thread{
public:
    Thread(const std::string& name);
    virtual ~Thread() {};
    
    virtual void startThreadRun();
    virtual void stopThreadRun();
    virtual void joinThread();

    bool exited() const { return mExited; }

protected:
    bool keepThreadRunning() const;
    virtual void threadMain() = 0;

private:

    struct ThreadRunner{
        ThreadRunner(Thread* thread) : mThread(thread) {}
        void operator() ()
            {
                mThread->runThread();
            }  
        Thread*   mThread;
    };

    void runThread();

private:
    const std::string      mName;
    bool                   mStartRun;
    bool                   mStopRun;
    boost::thread          mBoostThread;
    bool                   mExited;
};

#endif //THREAD_H

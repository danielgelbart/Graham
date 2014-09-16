#include "Utils.hpp"
#include "Logger.h"
#include "Thread.h"

using namespace std;

Thread::Thread(const string& name)
    : mName(name),
      mStartRun(false),
      mStopRun(false),
      mBoostThread(ThreadRunner(this)),
      mExited(false)
{
    LOG_INFO << "Finished initializing thread: " << mName;
}
    
void
Thread::startThreadRun()
{
    LOG_DEBUG << "Starting thread: " << mName;
    mStartRun = true;
}

void
Thread::stopThreadRun()
{
    LOG_INFO << "Stopping thread: " << mName;
    mStopRun = true;
}

void
Thread::joinThread()
{
    mBoostThread.join();
}

void
Thread::runThread()
{
    LOG_INFO << "Running thread: " << mName;
    while (!mStartRun){
        LOG_DEBUG << "Waiting for start signal: " << mName;
        sleep(1); 
    }
    threadMain();
    mExited = true;
    LOG_INFO << "Exiting thread: " << mName;
}

bool
Thread::keepThreadRunning() const
{
    return !mStopRun;
}




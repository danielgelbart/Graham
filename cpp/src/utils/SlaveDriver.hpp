#ifndef SLAVE_DRIVER_HPP
#define SLAVE_DRIVER_HPP

#include <vector>
#include <boost/thread/mutex.hpp>
#include "Logger.h"
#include "LockedQueue.hpp"
#include "Thread.h"
#include "Identifier.h"


template<class TaskIt>
class SlaveDriver{
private:
    class DummySlave;
    typedef Identifier<DummySlave, 'S'> SlaveId;

public:
    SlaveDriver(size_t numSlaves, const TaskIt begin, const TaskIt end,
                const std::string& name)
        : _numSlaves(numSlaves), _slaves(numSlaves),
          _iTask(begin), _begin(begin), _end(end), _name(name)
    {
        for (size_t i = 0; i < _slaves.size(); ++i){
            _slaves[i] = new Slave(*this, SlaveId(i));
            _slaves[i]->startThreadRun();
        }
    }

    ~SlaveDriver(){
        for (size_t i = 0; i < _slaves.size(); ++i){
            _slaves[i]->stopThreadRun();
            _slaves[i]->joinThread();
            delete _slaves[i];
        }
    }

    bool done() const { return _iTask == _end; }
    
private:

    class Slave : public Thread { 

    public:
        Slave(SlaveDriver& driver, SlaveId id) 
            : Thread(id.dbgval()), _driver(driver) {}

    protected:
        void threadMain(){
            while (keepThreadRunning()){
                typename TaskIt::value_type* task = _driver.getTask();
                if (task)
                    task->run();
                else    
                    break;
            }
        }
        
    private:
        SlaveDriver& _driver;
    };


    typename TaskIt::value_type* getTask(){
        boost::mutex::scoped_lock lock(_mutex);
        typename TaskIt::value_type* ret = 0;
        if (_iTask != _end){
            ret = &(*_iTask);
            ++_iTask;
        }
        size_t d = _iTask - _begin;
        size_t len = _end - _begin;
        if (len > 100 && d % (len / 20) == 0)
            LOG_INFO << "finished " 
                     << 100 * d / len
                     << "% of task \"" << _name << "\"";
        return ret;
    }



private:
    boost::mutex                   _mutex;
    size_t                         _numSlaves;
    std::vector<Slave*>            _slaves;
    TaskIt                         _iTask;
    TaskIt                         _begin;
    TaskIt                         _end;
    std::string                    _name;
};

template<class TaskIt>
void 
driveSlaves(size_t numSlaves, const TaskIt begin, const TaskIt end,
        size_t numTasks)
{
    if (numTasks > 0) {
        SlaveDriver<TaskIt> driver(numTasks < numSlaves ? std::max(numTasks,(size_t)1) : numSlaves, begin,
				   end, "");
        while (!driver.done())
            sleep(1);
    }
}

template<class TaskIt>
void 
driveSlaves(size_t numSlaves, const TaskIt begin, const TaskIt end,
        size_t numTasks, const std::string& name)
{
    if (numTasks > 0) {

        SlaveDriver<TaskIt> driver(numTasks < numSlaves ? std::max(numTasks,(size_t)1) : numSlaves, begin, end, name);
        while (!driver.done())
            sleep(1);
    }
}


#endif //SLAVE_DRIVER_HPP

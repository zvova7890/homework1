#ifndef TASKWORKER_H
#define TASKWORKER_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "TaskObject.h"


class TaskWorker
{
public:
    enum State {
        StateIdle,
        StateBusy
    };

public:
    TaskWorker();
    ~TaskWorker();

    bool  processTask(const TaskObject &);
    State state() const;
    void  quit();

    void setWorkerStateChangedNotifier(const std::function<void(TaskWorker::State)> &h);

protected:
    void worker();
    void setState(State state);
    void notifyStateChanged(State state);

private:
    std::mutex m_mutex;
    std::condition_variable m_condVar;
    std::thread m_thread;
    std::function<void(TaskWorker::State)> m_stateNotifierHandler;
    State m_state = StateIdle;
    TaskObject m_workingOn;
    bool m_quit = false;
};


#endif // TASKWORKER_H

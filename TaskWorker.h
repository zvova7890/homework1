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
    std::function<void(TaskWorker::State)> m_stateNotifierHandler;
    State m_state = StateIdle;
    bool m_quit = false;
    TaskObject m_workingOn;
    std::thread m_thread;
};


#endif // TASKWORKER_H

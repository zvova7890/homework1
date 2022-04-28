#include <iostream>
#include <assert.h>
#include "TaskWorker.h"


TaskWorker::TaskWorker() :
    m_thread(std::bind(&TaskWorker::worker, this))
{

}


TaskWorker::~TaskWorker()
{
    quit();
    m_thread.join();
}


bool TaskWorker::processTask(const TaskObject &task)
{
    /*std::cout << "Worker " << m_thread.get_id()
              << ", received task " << task.id
              << ", prio " << (int)task.prio
              << ", weight " << (int)task.weight
              << std::endl;*/

    std::unique_lock<std::mutex> _locker(m_mutex);
    if( state() != StateIdle ) {
        std::cerr << "processTask received task at non idle state!" << std::endl;
        return false;
    }

    m_workingOn = task;
    setState(StateBusy);
    m_condVar.notify_one();
    return true;
}


void TaskWorker::quit()
{
    m_quit = true;
    m_condVar.notify_one();
}


void TaskWorker::setWorkerStateChangedNotifier(const std::function<void(TaskWorker::State)> &h)
{
    m_stateNotifierHandler = h;
}


TaskWorker::State TaskWorker::state() const
{
    return m_state;
}


void TaskWorker::setState(State state)
{
    if( m_state != state ) {
        m_state = state;
        notifyStateChanged(state);
    }
}


void TaskWorker::notifyStateChanged(State state)
{
    if( m_stateNotifierHandler )
        m_stateNotifierHandler(state);
}


void TaskWorker::worker()
{
    while( m_quit == false ) {
        std::unique_lock<std::mutex> _locker(m_mutex);
        if( state() != StateBusy ) {
            m_condVar.wait(_locker);
            continue;
        }

        printf("Task %u started. prio %d, weight: %d\n", m_workingOn.id, m_workingOn.prio, m_workingOn.weight);
        std::this_thread::sleep_for(std::chrono::seconds(1 + m_workingOn.weight));

        printf("Task %u completed\n", m_workingOn.id);
        setState(StateIdle);
    }

    printf("TaskWorker thread end\n");
}

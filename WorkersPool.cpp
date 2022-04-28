#include <iostream>
#include <cassert>
#include "WorkersPool.h"


WorkersPool::WorkersPool(int maxWorkers) :
    m_pool(maxWorkers),
    m_idleWorkersCount(maxWorkers)
{
    for( TaskWorker &w : m_pool ) {
        w.setWorkerStateChangedNotifier([this](TaskWorker::State state) {
            if( state == TaskWorker::StateIdle ) {
                // here we protect counter and also m_hasIdleWorkerNotifier
                // because m_hasIdleWorkerNotifier can be changed at exit
                std::unique_lock<std::mutex> _locker(m_mutex);
                m_idleWorkersCount ++;
                if( m_idleWorkersCount > 0 && m_hasIdleWorkerNotifier)
                    m_hasIdleWorkerNotifier();

            } else {
                std::unique_lock<std::mutex> _locker(m_mutex);
                assert(m_idleWorkersCount > 0);
                m_idleWorkersCount --;
            }
        });
    }
}


bool WorkersPool::pushTask(const TaskObject &task)
{
    // find empty worker and push task for processing
    for( TaskWorker &w : m_pool ) {
        if( w.state() == TaskWorker::StateIdle ) {
            if( w.processTask(task) ) {
                return true;
            }
        }
    }

    return false;
}


bool WorkersPool::hasIdleWorker() const
{
    std::unique_lock<std::mutex> _locker(m_mutex);
    return m_idleWorkersCount > 0;
}


void WorkersPool::setHasIdleWorkerNotifier(const std::function<void()> &h)
{
    std::unique_lock<std::mutex> _locker(m_mutex);
    m_hasIdleWorkerNotifier = h;
}

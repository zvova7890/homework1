#include <iostream>
#include <functional>
#include <assert.h>
#include "QueueManager.h"


QueueManager::QueueManager(int maxPriority, int maxQueueLen, WorkersPool *workerPool) :
    m_workerPool(workerPool),
    m_queues(maxPriority),
    m_maxQueueLen(maxQueueLen)
{
    m_workerPool->setHasIdleWorkerNotifier(std::bind(&QueueManager::someWorkerBecomesIdle, this));
}


QueueManager::~QueueManager()
{
    m_workerPool->setHasIdleWorkerNotifier(nullptr);
}


// this will be called when workers pool arrived some idle task
void QueueManager::someWorkerBecomesIdle()
{
    m_condVar.notify_one();
}


bool QueueManager::pushNewTask(const TaskObject &task)
{
    if( task.prio >= m_queues.size() ) {
        std::cerr << "Invalid task prio " << task.prio << std::endl;
        return false;
    }

    std::unique_lock<std::mutex> _locker(m_mutex);
    if( m_taskPending >= m_maxQueueLen ) {
        std::cerr << "Max queue len is reached!" << std::endl;
        return false;
    }

    std::cout << "Push new task " << task.id
            << ", prio " << (int)task.prio
            << ", weight " << (int)task.weight
            << std::endl;

    m_queues[task.prio].push(task);
    m_taskPending ++;
    m_condVar.notify_one();
    return true;
}


void QueueManager::exec()
{
    std::cout << "QueueManager started" << std::endl;

    while(m_quit == false) {
        std::unique_lock<std::mutex> _locker(m_mutex);
        // no tasks or idle workers? wait for
        if( m_taskPending < 1 || !m_workerPool->hasIdleWorker() ) {
            m_condVar.wait(_locker);
        }

        // iterate over prio vector, from top to bottom
        // first place most priority tasks to the poll
        for(auto q = m_queues.rbegin(); q != m_queues.rend() && m_taskPending > 0; ++q) {
            // this prio level has no tasks, continue
            if( q->empty() )
                continue;

            if( !m_workerPool->hasIdleWorker() ) {
                //std::cout << "No idle worker found" << std::endl;
                break;
            }

            bool brk = false;
            while( !q->empty() )
            {
                TaskObject task = q->front();
                if( m_workerPool->pushTask(task) ) {
                    // task has pushed success, remove from queue
                    q->pop();
                    m_taskPending --;
                } else {
                    // push results fail, no reason to continue, go await for idle worker
                    brk = true;
                    break;
                }
            }

            if( brk )
                break;
        }
    }

    std::cout << "End of queue manager worker" << std::endl;
}


void QueueManager::quit()
{
    m_quit = true;
    m_condVar.notify_one();
}


bool QueueManager::isQuit() const
{
    return m_quit;
}

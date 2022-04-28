#ifndef QUEUEMANAGER_H
#define QUEUEMANAGER_H

#include <thread>
#include <queue>
#include "TaskWorker.h"
#include "WorkersPool.h"


class QueueManager
{
public:
    QueueManager(int maxPriority, int maxQueueLen, WorkersPool *workerPool);
    ~QueueManager();

    bool pushNewTask(const TaskObject &);
    void exec();
    void quit();
    bool isQuit() const;

protected:
    void someWorkerBecomesIdle();

private:
    WorkersPool *m_workerPool;
    std::mutex m_mutex;
    std::condition_variable m_condVar;
    std::vector<std::queue<TaskObject>> m_queues;
    uint32_t m_taskPending = 0;
    uint32_t m_maxQueueLen = 0;
    bool m_quit = false;
};

#endif // QUEUEMANAGER_H

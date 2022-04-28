#ifndef WORKERSPOOL_H
#define WORKERSPOOL_H

#include <functional>
#include <atomic>
#include <mutex>
#include "TaskWorker.h"


class WorkersPool
{
public:
    WorkersPool(int maxWorkers);

    bool pushTask(const TaskObject &task);
    bool hasIdleWorker() const;
    void setHasIdleWorkerNotifier(const std::function<void()> &h);

private:
    std::vector<TaskWorker> m_pool;
    uint32_t m_idleWorkersCount;
    std::function<void()> m_hasIdleWorkerNotifier;
    mutable std::mutex m_mutex;
};

#endif // WORKERSPOOL_H

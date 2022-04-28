#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include "QueueManager.h"
#include "TaskWorker.h"
#include "WorkersPool.h"

// maximum queue len for accepting task
#define MAX_QUEUE_LEN   10

// maximum delay between new task generating(seconds)
#define MAX_GEN_DELAY   5

// maximum priority levels
#define MAX_PRIO        10

// maxumim "weight" of task(seconds)
#define MAX_WEIGHT      5

// maximum worker threads count
#define MAX_WORKERS     5



QueueManager *queueManagerInstance = nullptr;

void sighandler(int signal)
{
    (void)signal;
    if( !queueManagerInstance )
        exit(0);

    queueManagerInstance->quit();
}


int main()
{
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    srand(time(nullptr));

    WorkersPool m_workerPool(MAX_WORKERS);
    QueueManager queueManager(MAX_PRIO, MAX_QUEUE_LEN, &m_workerPool);
    queueManagerInstance = &queueManager;

    std::thread lolka_generator([&queueManager]() {
        while( queueManager.isQuit() == false )
        {
            TaskObject task;
            task.id = TaskObject::newId();
            task.prio = rand() % MAX_PRIO;
            task.weight = rand() % MAX_WEIGHT;
            queueManager.pushNewTask(task);
            std::this_thread::sleep_for(std::chrono::seconds(0 + (rand() % MAX_GEN_DELAY)));
        }
    });


    queueManager.exec();
    lolka_generator.join();
    queueManagerInstance = nullptr;
    return 0;
}

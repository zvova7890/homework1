#include <iostream>
#include <list>
#include <stdlib.h>
#include <signal.h>
#include "QueueManager.h"
#include "TaskWorker.h"
#include "WorkersPool.h"

// maximum queue len for accepting task
#define MAX_QUEUE_LEN   1024

// maximum delay between new task generating(seconds)
#define MAX_GEN_DELAY   5

// maximum priority levels
#define MAX_PRIO        10

// maxumim "weight" of task(seconds)
#define MAX_WEIGHT      5

// maximum worker threads count
#define MAX_WORKERS     5

// maximum publicator threads count
#define MAX_PUBLICATORS 10



QueueManager *queueManagerInstance = nullptr;

void sighandler(int signal)
{
    (void)signal;
    if( !queueManagerInstance )
        exit(0);

    queueManagerInstance->quit();
}


void publicator_thread()
{
    while( queueManagerInstance->isQuit() == false )
    {
        TaskObject task;
        task.id = TaskObject::newId();
        task.prio = rand() % MAX_PRIO;
        task.weight = rand() % MAX_WEIGHT;
        queueManagerInstance->pushNewTask(task);
        std::this_thread::sleep_for(std::chrono::seconds(1 + (rand() % MAX_GEN_DELAY)));
    }
}


int main()
{
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    srand(time(nullptr));

    std::list<std::thread *> publicators;
    WorkersPool m_workerPool(MAX_WORKERS);
    QueueManager queueManager(MAX_PRIO, MAX_QUEUE_LEN, &m_workerPool);
    queueManagerInstance = &queueManager;

    for( int i = 0; i < MAX_PUBLICATORS; ++i ) {
        publicators.push_back( new std::thread(publicator_thread) );
    }

    // exec queue manager(will hang until quit)
    queueManager.exec();

    // wait for publicators exit
    for( auto thread : publicators ) {
        thread->join();
        delete thread;
    }
    queueManagerInstance = nullptr;
    return 0;
}

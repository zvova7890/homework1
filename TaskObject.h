#ifndef TASKOBJECT_H
#define TASKOBJECT_H

#include <inttypes.h>


struct TaskObject
{
    uint32_t id;
    uint8_t prio;
    uint8_t weight;

    static uint32_t newId();
};

#endif // TASKOBJECT_H

#include <atomic>
#include "TaskObject.h"

static std::atomic<uint32_t> task_id_counter = 0;


uint32_t TaskObject::newId()
{
    return task_id_counter++;
}

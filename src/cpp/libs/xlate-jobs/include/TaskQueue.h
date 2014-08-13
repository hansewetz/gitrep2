#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H__
#include "asio-extensions/simple_queue.h"
#include "xlate-jobs/TranslateRequest.h"
namespace xlate{

// typedef ofr a task queue
using TaskQueue=boost::asio::simple_queue<TranslateRequest>;

}
#endif

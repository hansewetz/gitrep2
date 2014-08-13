#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H__
#include "asio-extensions/simple_queue.h"
#include "xlate-jobs/TranslationTask.h"
#include <memory>
namespace xlate{

// typedef ofr a task queue
using TaskQueue=boost::asio::simple_queue<std::shared_ptr<TranslationTask>>;

}
#endif

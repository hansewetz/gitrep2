#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H__
#include "asio-extensions/queue_listener.h"
#include "asio-extensions/queue_sender.h"
#include "asio-extensions/simple_queue.h"
#include "xlate-jobs/TranslationTask.h"
#include <memory>
namespace xlate{

// typedef of a task queue
using TaskQueue=boost::asio::simple_queue<std::shared_ptr<TranslationTask>>;

// typedef of q sender
using TaskQueueSender=boost::asio::simple_queue_sender<std::shared_ptr<TranslationTask>>;

// typedef of q listener
using TaskQueueListener=boost::asio::simple_queue_listener<std::shared_ptr<TranslationTask>>;
}
#endif

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H__
#include "asio-extensions/queue_listener.hpp"
#include "asio-extensions/queue_sender.hpp"
#include "asio-extensions/simple_queue.hpp"
#include "xlate-jobs/TranslationTask.h"
#include <memory>
namespace xlate{

// typedef of a task queue
using TaskQueue=boost::asio::simple_queue<std::shared_ptr<TranslationTask>>;

// typedef of q sender
using TaskQueueSender=boost::asio::queue_sender<TaskQueue>;

// typedef of q listener
using TaskQueueListener=boost::asio::queue_listener<TaskQueue>;
}
#endif


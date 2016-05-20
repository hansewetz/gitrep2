// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H__
#include "asio-extensions/queue_listener.hpp"
#include "asio-extensions/queue_sender.hpp"
#include "asio-extensions/detail/queue_interface_base.hpp"
#include "xlate-jobs/TranslationTask.h"
#include <memory>
namespace xlate{

// typedef of a task queue
using TaskQueueDeq=boost::asio::detail::base::queue_interface_deq<std::shared_ptr<TranslationTask>>;
using TaskQueueEnq=boost::asio::detail::base::queue_interface_enq<std::shared_ptr<TranslationTask>>;

// typedef of q sender
using TaskQueueSender=boost::asio::queue_sender<TaskQueueEnq>;

// typedef of q listener
using TaskQueueListener=boost::asio::queue_listener<TaskQueueDeq>;
}
#endif


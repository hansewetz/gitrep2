// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#ifndef __JOB_QUEUE_H__
#define __JOB_QUEUE_H__
#include "asio-extensions/queue_listener.hpp"
#include "asio-extensions/queue_sender.hpp"
#include "asio-extensions/simple_queue.hpp"
#include "xlate-jobs/TranslationJob.h"
#include <memory>
namespace xlate{

// typedef of a task queue
using JobQueue=boost::asio::simple_queue<std::shared_ptr<TranslationJob>>;

// typedef of q sender
using JobQueueSender=boost::asio::queue_sender<JobQueue>;

// typedef of q listener
using JobQueueListener=boost::asio::queue_listener<JobQueue>;
}
#endif

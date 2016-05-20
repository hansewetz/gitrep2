// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#ifndef __JOB_QUEUE_H__
#define __JOB_QUEUE_H__
#include "asio-extensions/queue_listener.hpp"
#include "asio-extensions/queue_sender.hpp"
#include "asio-extensions/detail/queue_interface_base.hpp"
#include "xlate-jobs/TranslationJob.h"
#include <memory>
namespace xlate{

// typedef of a task queue
using JobQueueDeq=boost::asio::detail::base::queue_interface_deq<std::shared_ptr<TranslationJob>>;
using JobQueueEnq=boost::asio::detail::base::queue_interface_enq<std::shared_ptr<TranslationJob>>;

// typedef of q sender
using JobQueueSender=boost::asio::queue_sender<JobQueueEnq>;

// typedef of q listener
using JobQueueListener=boost::asio::queue_listener<JobQueueDeq>;
}
#endif

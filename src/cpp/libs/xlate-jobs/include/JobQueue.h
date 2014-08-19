#ifndef __JOB_QUEUE_H__
#define __JOB_QUEUE_H__
#include "asio-extensions/queue_listener.h"
#include "asio-extensions/queue_sender.h"
#include "asio-extensions/simple_queue.h"
#include "xlate-jobs/TranslationJob.h"
#include <memory>
namespace xlate{

// typedef of a task queue
using JobQueue=boost::asio::simple_queue<std::shared_ptr<TranslationJob>>;

// typedef of q sender
using JobQueueSender=boost::asio::simple_queue_sender<std::shared_ptr<TranslationJob>>;

// typedef of q listener
using JobQueueListener=boost::asio::simple_queue_listener<std::shared_ptr<TranslationJob>>;
}
#endif

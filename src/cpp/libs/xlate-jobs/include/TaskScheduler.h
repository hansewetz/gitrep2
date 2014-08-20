#ifndef __TASK_SCHEDULER_H__
#define __TASK_SCHEDULER_H__
#include "xlate-jobs/JobQueue.h"
#include "xlate-jobs/TaskQueue.h"
#include <memory>
#include <deque>
#include <boost/asio.hpp>
namespace xlate{

// forward decl.
class TranslationTask;

// class scheduling tasks taken from a job repository
class TaskScheduler{
public:
  TaskScheduler(boost::asio::io_service&ios,std::shared_ptr<JobQueue>qjob,std::shared_ptr<TaskQueue>qtask);
  TaskScheduler(TaskScheduler const&)=delete;
  TaskScheduler(TaskScheduler&&)=default;
  TaskScheduler&operator=(TaskScheduler const&)=delete;
  TaskScheduler&operator=(TaskScheduler&&)=default;
  ~TaskScheduler()=default;

  // start listening on events
  void run();
private:
  // asio stuff
  boost::asio::io_service&ios_;
  std::shared_ptr<JobQueueListener>qjobListener_;
  std::shared_ptr<TaskQueueSender>qtaskSender_;

  // jobs currently being processed
  std::deque<std::shared_ptr<TranslationJob>>jobs_;

  // flag specifying if we are waiting for task queue to unblock
  bool waiting4unblock_;

  // helper functions
  void waitForUnblock();
  void waitForNewJob();

  // helper functions
  void waitUnblockHandler(boost::system::error_code const&ec);
  void newJobHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationJob>job);

  // helper functions - scheduling
  std::shared_ptr<TranslationTask>nextTask();
  bool hasNextTask()const;
};
}
#endif

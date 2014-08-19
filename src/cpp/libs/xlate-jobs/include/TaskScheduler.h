#ifndef __TASK_SCHEDULER_H__
#define __TASK_SCHEDULER_H__
#include "xlate-jobs/TaskQueue.h"
#include <iosfwd>
#include <memory>
#include <boost/asio.hpp>
namespace xlate{

// forward decl
class TranslationJobRepository;
class TranslationTask;

// class scheduling tasks taken from a job repository
class TaskScheduler{
public:
  TaskScheduler(boost::asio::io_service&ios,std::shared_ptr<TranslationJobRepository>jobrep,std::shared_ptr<TaskQueue>q);
  TaskScheduler(TaskScheduler const&)=delete;
  TaskScheduler(TaskScheduler&&)=default;
  TaskScheduler&operator=(TaskScheduler const&)=delete;
  TaskScheduler&operator=(TaskScheduler&&)=default;
  ~TaskScheduler()=default;

  // debug print operator
  friend std::ostream&operator<<(std::ostream&os,TaskScheduler const&tc);
private:
  boost::asio::io_service&ios_;
  std::shared_ptr<TranslationJobRepository>jobrep_;
  std::shared_ptr<TaskQueue>q_;
  TaskQueueSender qsender_;

  // handlers
  void waitSend(boost::system::error_code const&ec);
};
}
#endif

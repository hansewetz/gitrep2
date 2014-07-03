#ifndef __TASK_SCHEDULER_H__
#define __TASK_SCHEDULER_H__
#include <iosfwd>
#include <memory>
#include <mutex>
namespace xlate{

// forward decl
class TranslationJobRepository;
class TaskQueue;

// class scheduling tasks taken from a job repository
class TaskScheduler{
public:
  TaskScheduler(std::shared_ptr<TranslationJobRepository>jobrep,std::shared_ptr<TaskQueue>taskq);
  TaskScheduler(TaskScheduler const&)=delete;
  TaskScheduler(TaskScheduler&&)=default;
  TaskScheduler&operator=(TaskScheduler const&)=delete;
  TaskScheduler&operator=(TaskScheduler&&)=default;
  ~TaskScheduler()=default;

  // run task collector (moves tasks from task queue into job repository)
  void operator()();

  // controll functions for scheduler
  void terminate();

  // debug print operator
  friend std::ostream&operator<<(std::ostream&os,TaskScheduler const&tc);
private:
  std::shared_ptr<TranslationJobRepository>jobrep_;
  std::shared_ptr<TaskQueue>taskq_;

  bool done_;
  mutable std::mutex mtx_;
};
}
#endif

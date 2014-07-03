#ifndef __TASK_COLLECTOR_H__
#define __TASK_COLLECTOR_H__
#include <iosfwd>
#include <memory>
namespace xlate{

// forward decl
class TranslationJobRepository;
class TaskQueue;

// class collecting tasks form a queue and forwarding them to a job repository
class TaskCollector{
public:
  TaskCollector(std::shared_ptr<TranslationJobRepository>jobrep,std::shared_ptr<TaskQueue>taskq);
  TaskCollector(TaskCollector const&)=delete;
  TaskCollector(TaskCollector&&)=default;
  TaskCollector&operator=(TaskCollector const&)=delete;
  TaskCollector&operator=(TaskCollector&&)=default;
  ~TaskCollector()=default;

  // run task collector (moves tasks from task queue into job repository)
  void operator()();

  // debug print operator
  friend std::ostream&operator<<(std::ostream&os,TaskCollector const&tc);
private:
  std::shared_ptr<TranslationJobRepository>jobrep_;
  std::shared_ptr<TaskQueue>taskq_;
};
}
#endif

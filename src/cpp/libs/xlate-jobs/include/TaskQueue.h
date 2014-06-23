#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H__
#include <iosfwd>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
namespace xlate{

// forward decl
class TranslationTask;

// queue holding shared pointers to TranslationTask objects
class TaskQueue{
public:
  // ctors, dtor
  TaskQueue(std::size_t maxsize);
  TaskQueue(TaskQueue const&)=delete;
  TaskQueue(TaskQueue&&)=default;
  TaskQueue&operator=(TaskQueue&&)=default;
  ~TaskQueue()=default;

  // operations on queue
  std::size_t size()const ;
  bool enq(std::shared_ptr<TranslationTask>task);
  std::shared_ptr<TranslationTask>deq(bool block);
  bool wait(bool block)const;

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  std::queue<std::shared_ptr<TranslationTask>>queue_;
  mutable std::mutex mtx_;
  mutable std::condition_variable cond_;
  std::size_t maxsize_;
};
// debug print operator
std::ostream&operator<<(std::ostream&os,TaskQueue const&t);
}
#endif

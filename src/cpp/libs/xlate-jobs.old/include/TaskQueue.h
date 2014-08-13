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
  TaskQueue&operator=(TaskQueue const&)=delete;
  TaskQueue&operator=(TaskQueue&&)=default;
  ~TaskQueue()=default;

  // operations on queue
  bool enq(std::shared_ptr<TranslationTask>task);
  std::shared_ptr<TranslationTask>deq(bool block);
  void blockDeq();
  void unblockDeq();

  // queries on queue
  std::size_t size()const ;
  bool wait(bool block)const;
  bool deqIsBlocked()const;

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  // sync stuff
  mutable std::mutex mtx_;
  mutable std::condition_variable cond_;

  // state of queue
  std::queue<std::shared_ptr<TranslationTask>>queue_;
  std::size_t maxsize_;
  mutable std::size_t nwaiting_;
  bool blockdeq_;
};
// debug print operator
std::ostream&operator<<(std::ostream&os,TaskQueue const&t);
}
#endif

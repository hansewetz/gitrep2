#include "xlate-jobs/TaskQueue.h"
#include "xlate-jobs/TranslationTask.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctor
TaskQueue::TaskQueue(size_t maxsize):maxsize_(maxsize){
}
// get #of items in queue
size_t TaskQueue::size()const {
  lock_guard<mutex>lock(mtx_);
  return queue_.size();
}
// enqueue an item
bool TaskQueue::enq(shared_ptr<TranslationTask>task){
  {
    lock_guard<mutex>lock(mtx_);
    if(queue_.size()>=maxsize_)return false;
    queue_.push(task);
  }
  cond_.notify_all();
  return true;
}
// dequeue an item (block if needed)
shared_ptr<TranslationTask>TaskQueue::TaskQueue::deq(bool block){
  // wait blocking or wait for 0 duration
  unique_lock<mutex>lock(mtx_);
  if(block)cond_.wait(lock,[&](){return !queue_.empty();});
  else cond_.wait_for(lock,chrono::milliseconds(0),[&](){return !queue_.empty();});

  // if queue empty return null ptr
  if(queue_.empty())return shared_ptr<TranslationTask>(nullptr);

  // check if we have a message
  shared_ptr<TranslationTask>ret{queue_.front()};
  queue_.pop();
  return ret;
}
// wait until a task arrives - returnsd true if so, else false
bool TaskQueue::wait(bool block)const{
  unique_lock<mutex>lock(mtx_);
  if(block)cond_.wait(lock,[&](){return !queue_.empty();});
  else cond_.wait_for(lock,chrono::milliseconds(0),[&](){return !queue_.empty();});

  // if queue not emoty we have a task
  return !queue_.empty();
}
// print function
ostream&TaskQueue::print(ostream&os)const{
   return os<<"#items: "<<size();
}
// debug print operator
ostream&operator<<(ostream&os,TaskQueue const&t){
  return t.print(os);
}
}

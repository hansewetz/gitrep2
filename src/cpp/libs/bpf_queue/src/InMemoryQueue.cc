// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include "bpf_queue/InMemoryQueue.h"
#include "bpf_queue/Message.h"
#include "utils/utility.h"
#include <boost/log/trivial.hpp>
#include <chrono>
#include <stdexcept>
using namespace std;
namespace bpf{

// ctor (must set started)
InMemoryQueue::InMemoryQueue(QueueId const&id,bool start):Queue(id),started_(start){
}
// identified of this queue type
string InMemoryQueue::type()const{
  return "InMemoryQueue";
}
// start queue
void InMemoryQueue::start(){
  lock_guard<mutex>lock(mtx_);
  started_=true;
  cond_.notify_all();
}
// stop queue
void InMemoryQueue::stop(){
  lock_guard<mutex>lock(mtx_);
  started_=false;
  cond_.notify_all();
}
// check if queue is running
bool InMemoryQueue::started()const{
  lock_guard<mutex>lock(mtx_);
  return started_;
}
// check if queue is empty
bool InMemoryQueue::empty()const{
  lock_guard<mutex>lock(mtx_);
  return queue_.empty();
}
// get #of elements in queue
size_t InMemoryQueue::size()const{
  lock_guard<mutex>lock(mtx_);
  return queue_.size();
}
// enqueue a message
bool InMemoryQueue::enq(shared_ptr<Message>msg){
  {
    lock_guard<mutex>lock(mtx_);
    if(!started_)return false;
    queue_.push(msg);
  }
  cond_.notify_all();
  return true;
}
// dequeue a message (block or return if no message)
shared_ptr<Message>InMemoryQueue::deq(bool block){
  // wait blocking or wait for 0 duration
  unique_lock<mutex>lock(mtx_);
  if(block)cond_.wait(lock,[&](){return !queue_.empty()||!started_;});
  else cond_.wait_for(lock,chrono::milliseconds(0),[&](){return !queue_.empty()||!started_;});

  // if we are stopped return null ptr
  if(!started_)return shared_ptr<Message>(nullptr);

  // if queue empty return null ptr
  if(queue_.empty())return shared_ptr<Message>(nullptr);

  // check if we have a message
  shared_ptr<Message>ret{queue_.front()};
  queue_.pop();
  return ret;
}
// print function
ostream&InMemoryQueue::print(ostream&os)const{
  lock_guard<mutex>lock(mtx_);
  return os<<"type: "<<type()<<"qid: ["<<id()<<"], started: "<<boolalpha<<started_;
}
}

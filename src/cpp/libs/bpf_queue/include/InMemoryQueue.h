// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#ifndef __IN_MEMORY_QUEUE_H__
#define __IN_MEMORY_QUEUE_H__
#include "bpf_queue/Queue.h"
#include <memory>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
namespace bpf{

// forward decl
class Message;

// in memory queue
class InMemoryQueue:public Queue{
public:
  // bring in Queue to this nm
  using Queue::Queue;

  // ctors,assign,dtor (movable only)
  InMemoryQueue(QueueId const&,bool start);
  ~InMemoryQueue()=default;

  // enq and deq
  virtual std::string type()const;
  virtual void start()override;
  virtual void stop()override;
  virtual bool started()const override;
  virtual bool empty()const override;
  virtual std::size_t size()const override;
  virtual bool enq(std::shared_ptr<Message>msg)override;
  virtual std::shared_ptr<Message>deq(bool block)override;

  // print function
  virtual std::ostream&print(std::ostream&os)const override;
private:
  std::queue<std::shared_ptr<Message>>queue_;
  mutable std::mutex mtx_;
  mutable std::condition_variable cond_;
  bool started_;
};
}
#endif

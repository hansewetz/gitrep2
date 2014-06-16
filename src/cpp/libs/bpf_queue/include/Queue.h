#ifndef __QUEUE_H__
#define __QUEUE_H__
#include "bpf_utils/Id.h"
#include <iosfwd>
#include <memory>
#include <functional>
namespace bpf{

// forward decl
class Message;

// identifier type for class
class Queue;
using QueueId=Id<Queue>;

// message queue class
class Queue:public std::enable_shared_from_this<Queue>{
public:
  // print operator
  friend std::ostream&operator<<(std::ostream&,Queue const&);

  // ctors,assign,dtor (movable only)
  Queue(QueueId const&);
  Queue(Queue const&)=delete;
  Queue(Queue&&)=default;
  Queue&operator=(Queue const&)=delete;
  Queue&operator=(Queue&&)=default;
  virtual ~Queue()=default;

  // getters
  QueueId const&id()const;

  // queue functions to be implemented in derived classes
  virtual std::string type()const=0;
  virtual void start()=0;
  virtual void stop()=0;
  virtual bool started()const=0;
  virtual bool empty()const=0;
  virtual std::size_t size()const=0;
  virtual bool enq(std::shared_ptr<Message>)=0;
  virtual std::shared_ptr<Message>deq(bool block)=0;

  // print function
  virtual std::ostream&print(std::ostream&)const=0;
private:
  QueueId id_;
};
}
#endif

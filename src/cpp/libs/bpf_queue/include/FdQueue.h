#ifndef __FD_QUEUE_H__
#define __FD_QUEUE_H__
#include "bpf_queue/Queue.h"
#include <memory>
#include <queue>
#include <functional>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
namespace bpf{

// forward decl
class Message;

// in memory queue
class FdQueue:public Queue{
public:
// helper class used for removing locks in ctor
struct LockRemover{
  LockRemover(bool removeLocks,std::string const&qname);
};
  // bring in Queue to this nm
  using Queue::Queue;

  // ctors,assign,dtor (movable only)
  FdQueue(QueueId const&,bool start,int fdread,int fdwrite,bool removelocks);
  ~FdQueue()=default;

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
  // is queue started?
  bool started_;

  // read.write fds
  int fdread_;
  int fdwrite_;

  // mutex protection for multiple reader/writers
  LockRemover lockRemover;
  mutable boost::interprocess::named_mutex mtxRead_;
  mutable boost::interprocess::named_mutex mtxWrite_;

  // helper functions
  void setfdnonblock(int fd);

  // get name of mutex/condition variable for this queue name
  static void removeLockVariables(std::string const&qname);
  static std::string getMutexName(std::string const&qname,bool reader);
};
}
#endif

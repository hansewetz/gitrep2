#ifndef __POLL_DIR_QUEUE_H__
#define __POLL_DIR_QUEUE_H__
#include "bpf_queue/Queue.h"
#include <boost/filesystem.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <memory>
#include <list>
namespace bpf{

// forward decl
class Message;

// in memory queue
// (queue is inter-process safe when for enq/deq)
class PolldirQueue:public Queue{
// helper class used for removing locks in ctor
struct LockRemover{
  LockRemover(bool removeLocks,boost::filesystem::path const&dir);
};
public:
  // bring in Queue to this nm
  using Queue::Queue;

  // ctors,assign,dtor (movable only)
  PolldirQueue(QueueId const&,bool start,std::size_t pollrateMsec,boost::filesystem::path const&dir,bool removeLocks);
  ~PolldirQueue()=default;

  // enq and deq
  virtual std::string type()const;
  virtual void start()override;
  virtual void stop()override;
  virtual bool started()const override;
  virtual bool empty()const override;
  virtual std::size_t size()const override;
  virtual bool enq(std::shared_ptr<Message>msg)override;
  virtual std::shared_ptr<Message>deq(bool block)override;

  // special functions for PolldirQueue class
  static void removeLockVariables(boost::filesystem::path const&dir);

  // print function
  virtual std::ostream&print(std::ostream&os)const override;
private:
  // user controlled state
  bool started_;
  std::size_t pollrateMsec_;
  boost::filesystem::path dir_;

  // internally controlled state
  LockRemover lockRemover;
  mutable boost::interprocess::named_mutex mtx_;
  mutable boost::interprocess::named_condition cond_;
  mutable std::list<boost::filesystem::path>cache_;

  // helper functions (lock must be held when calling these functions)
  void writeMessage(std::shared_ptr<Message>msg)const;
  std::shared_ptr<Message>readMessage(boost::filesystem::path const&filename)const;
  bool emptyNolock()const;
  std::pair<bool,boost::filesystem::path>nextFile()const;
  std::list<boost::filesystem::path>allFiles()const;
  void fillCache(bool)const;
  void cleanCache()const;

  // get name of mutex/condition variable for this queue name
  static std::string getMutexName(boost::filesystem::path const&dir);
  static std::string getCondName(boost::filesystem::path const&dir);
};
}
#endif

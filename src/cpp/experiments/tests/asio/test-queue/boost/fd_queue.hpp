#ifndef __FD_QUEUE_H__
#define __FD_QUEUE_H__
#include <utility>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace boost{
namespace asio{

// a simple thread safe queueonly one instance must exist of the queue object in a process
// the queue has no concept of size since objects are never store in the queue - obbject are read when deq() is called and written when enq() is called`
// the queue cuold potemtially be used for inmterprocess communication via pipes
template<typename T,typename DESER,typename SERIAL>
class fd_queue{
public:
  // typedef for value stored in queue
  // (need this so we can create an item with default ctor)
  using value_type=T;

  // ctors,assign,dtor
  fd_queue(int fd_deq,int fd_enq,DESER deser,SERIAL serial):fd_deq_(fd_deq),fd_enq_(fd_enq),deser_(deser),serial_(serial){}
  fd_queue(fd_queue const&)=delete;
  fd_queue(fd_queue&&)=default;
  fd_queue&operator=(fd_queue const&)=delete;
  fd_queue&operator=(fd_queue&&)=default;
  ~fd_queue()=default;

  // put a message into queue
  bool enq(T t,boost::system::error_code&ec){
    // NOTE! Not yet done
  }
  // put a message into queue - timeout if waiting too long
  bool timed_enq(T t,std::size_t ms,boost::system::error_code&ec){
    // NOTE! Not yet done
  }
  // wait until we can put a message in queue
  bool wait_enq(boost::system::error_code&ec){
    // NOTE! Not yet done
  }
  // wait until we can put a message in queue - timeout if waiting too long
  bool timed_wait_enq(std::size_t ms,boost::system::error_code&ec){
    // NOTE! Not yet done
  }
  // dequeue a message (return.first == false if deq() was disabled)
  std::pair<bool,T>deq(boost::system::error_code&ec){
    // NOTE! Not yet done
  }
  // dequeue a message (return.first == false if deq() was disabled) - timeout if waiting too long
  std::pair<bool,T>timed_deq(std::size_t ms,boost::system::error_code&ec){
    // NOTE! Not yet done
  }
  // wait until we can retrieve a message from queue
  bool wait_deq(boost::system::error_code&ec){
    // NOTE! Not yet done
  }
  // wait until we can retrieve a message from queue -  timeout if waiting too long
  bool timed_wait_deq(std::size_t ms,boost::system::error_code&ec){
    // NOTE! Not yet done
  }
  // cancel deq operations (will also release blocking threads)
  void disable_deq(bool disable){
    std::unique_lock<std::mutex>lock(mtx_);
    deq_enabled_=!disable;
    cond_.notify_all();
  }
  // cancel enq operations (will also release blocking threads)
  void disable_enq(bool disable){
    std::unique_lock<std::mutex>lock(mtx_);
    enq_enabled_=!disable;
    cond_.notify_all();
  }
private:
  // file descriptors to read from and to write to
  int fd_deq_;
  int fd_enq_;

  // synchronization variables
  mutable std::mutex mtx_;
  mutable std::condition_variable cond_;

  // variables controlling if queue is enabled or not
  bool deq_enabled_=true;
  bool enq_enabled_=true;

  // serailiaser/de-serialiser
  DESER deser_;
  SERIAL serial_;
};
}
}
#endif

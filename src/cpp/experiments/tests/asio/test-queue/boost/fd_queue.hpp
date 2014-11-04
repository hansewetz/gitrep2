#ifndef __FD_QUEUE_H__
#define __FD_QUEUE_H__
#include "detail/queue_support.hpp"
#include <utility>
#include <list>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace boost{
namespace asio{

// a simple thread safe queue
// typically two queue objects would exist possibly talking through a pipe
// the queue stores objects inside the queue - object are read when deq() is called and written when enq() is called`
// any entity could tread/write from/to the queue as long as serialization/desrialization protocols are folloewed
// fd's must be blocking so that we don't read partial bits and pieces of a message
template<typename T,typename DESER,typename SERIAL>
class fd_queue{
public:
  // typedef for value stored in queue
  // (need this so we can create an item with default ctor)
  using value_type=T;

  // ctors,assign,dtor
  fd_queue(std::size_t maxsize,int fd_deq,int fd_enq,DESER deser,SERIAL serial):maxsize_(maxsize),fd_deq_(fd_deq),fd_enq_(fd_enq),deser_(deser),serial_(serial){}
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
    // check if we have items in cache
    std::unique_lock<std::mutex>lock(mtx_);
    if(!cache_.empty()){
      T ret{cache_.front()};
      cache_.pop_front();
      return std::make_pair(true,ret);
    }
    // do a blocking call until we get an error or something on fd_deq_ (fd_deq_ might close on us)
    std::shared_ptr<std::istream>is{asio::detail::queue_support::selectFd(fd_deq_,0,ec)};
    if(ec!=boost::system::error_code())return std::make_pair(false,T{});
    
    // read from stream (this is a blocking read - we read until we are done)
    T ret{deser_(*is)};

    // we are done - notify any waiting threads
    cond_.notify_all();
    return std::make_pair(true,ret);
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
  // set max size of queue
  void set_maxsize(std::size_t maxsize){
    std::unique_lock<std::mutex>lock(mtx_);
    maxsize_=maxsize;
  }
  // check if queue is empty
  bool empty()const{
    std::unique_lock<std::mutex>lock(mtx_);
    return cache_.empty();
  }
  // check if queue is full
  bool full()const{
    std::unique_lock<std::mutex>lock(mtx_);
    return cache_.size()==maxsize_;
  }
  // get #of items in queue
  std::size_t size()const{
    std::unique_lock<std::mutex>lock(mtx_);
    return cache_.size();
  }
  // get max items in queue
  std::size_t maxsize()const{
    std::unique_lock<std::mutex>lock(mtx_);
    return maxsize_;
  }
private:
  // file descriptors to read from and to write to
  int fd_deq_;
  int fd_enq_;

  // max size of queue
  std::size_t maxsize_;

  // buffer of queued items
  std::list<T>cache_;

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

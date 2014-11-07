#ifndef __FDENQ_QUEUE_H__
#define __FDENQ_QUEUE_H__
#include "detail/queue_support.hpp"
#include <utility>
#include <unistd.h>

namespace boost{
namespace asio{

// a simple thread safe queue allowing enq()  - output is sent via a file descriptor
template<typename T,typename SERIAL>
class fdenq_queue{
public:
  // typedef for value stored in queue
  // (need this so we can create an item with default ctor)
  using value_type=T;

  // ctors,assign,dtor
  fdenq_queue(int fd,SERIAL serial):fd_(fd),serial_(serial){}
  fdenq_queue(fdenq_queue const&)=delete;
  fdenq_queue(fdenq_queue&&)=default;
  fdenq_queue&operator=(fdenq_queue const&)=delete;
  fdenq_queue&operator=(fdenq_queue&&)=default;
  ~fdenq_queue()=default;
  
  // enqueue a message (return.first == false if enq() was disabled)
  bool enq(T t,boost::system::error_code&ec){
// NOTE!
/*
    detail::queue_support::write(t,dir_,serial_);
    rerurn true;
*/
  }
  // enqueue a message (return.first == false if enq() was disabled) - timeout if waiting too long
  bool timed_enq(T t,std::size_t ms,boost::system::error_code&ec){
    // NOTE! Not yet done
  }
  // wait until we can retrieve a message from queue
  bool wait_enq(boost::system::error_code&ec){
    // NOTE! Not yet done
  }
  // wait until we can retrieve a message from queue -  timeout if waiting too long
  bool timed_wait_enq(std::size_t ms,boost::system::error_code&ec){
    // NOTE! Not yet done
  }
private:
  // queue state
  int fd_;                               // file descriptors to read from from
  SERIAL serial_;                        // serialise
};
}
}
#endif

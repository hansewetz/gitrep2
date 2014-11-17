#ifndef __FDDEQ_QUEUE_H__
#define __FDDEQ_QUEUE_H__
#include "detail/queue_support.hpp"
#include <utility>
#include <unistd.h>

namespace boost{
namespace asio{

// a simple queue based on sending messages separated by '\n'
// (if sending objects first serialise the object, the base64 encode it in the serialiser)
template<typename T,typename DESER>
class fddeq_queue{
public:
  // typedef for value stored in queue
  // (need this so we can create an item with default ctor)
  using value_type=T;

  // ctors,assign,dtor
  fddeq_queue(int fdread,DESER deser):fdread_(fdread),deser_(deser){}
  fddeq_queue(fddeq_queue const&)=delete;
  fddeq_queue(fddeq_queue&&)=default;
  fddeq_queue&operator=(fddeq_queue const&)=delete;
  fddeq_queue&operator=(fddeq_queue&&)=default;
  ~fddeq_queue()=default;
  
  // dequeue a message (return.first == false if deq() was disabled)
  std::pair<bool,T>deq(boost::system::error_code&ec){
    // create an input stream and deserialise into an object
    std::shared_ptr<std::istream>is{detail::queue_support::makefd_istream(fdread_,false)};
    T ret{deser_(*is)};
    if(ec!=boost::system::error_code())return std::make_pair(false,ret);
    return make_pair(true,ret);
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
  // get file descriptor utility functions
  // (note: be carefulll when using - never close the fd unless you know no operation is in progress)
  int getfd()const{
    return fdread_;
  }
private:
  // queue state
  int fdread_;                           // file descriptors to read from from
  DESER deser_;                          // de-serialiser
};
}
}
#endif

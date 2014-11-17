#ifndef __FDDEQ_QUEUE_H__
#define __FDDEQ_QUEUE_H__
#include "detail/queue_support.hpp"
#include <utility>
#include <unistd.h>

namespace boost{
namespace asio{

// a simple thread safe queue allowing deq() from a file descriptor
// (the queue is menat to show how to implement a simple queue between two processses using a pipe/fork)
// (the queue doe snot use threads to populate a cache - insteda it reads when deq() is called)
template<typename T,typename DESER>
class fddeq_queue{
public:
  // typedef for value stored in queue
  // (need this so we can create an item with default ctor)
  using value_type=T;

  // ctors,assign,dtor
  fddeq_queue(int fd,DESER deser):fd_(fd),deser_(deser){}
  fddeq_queue(fddeq_queue const&)=delete;
  fddeq_queue(fddeq_queue&&)=default;
  fddeq_queue&operator=(fddeq_queue const&)=delete;
  fddeq_queue&operator=(fddeq_queue&&)=default;
  ~fddeq_queue()=default;
  
  // dequeue a message (return.first == false if deq() was disabled)
  std::pair<bool,T>deq(boost::system::error_code&ec){
    // read from stream (this is a blocking read - we read until we are done)
    T ret{deserialize(0,ec)};
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
private:
  // deserialise an object from an fd stream
  T deserialize(std::size_t ms,boost::system::error_code&ec){
    ec=boost::system::error_code();
    T ret;
    while(true){
      // setup to listen on fd descriptor
      fd_set input;
      FD_ZERO(&input);
      FD_SET(fd_,&input);
      int maxfd=fd_;
  
      // setup for timeout
      struct timeval tmo;
      if(ms>0){
        tmo.tv_sec=ms/1000;
        tmo.tv_usec=(ms%1000)*1000;
      }
      // block on select - timeout if configured
      assert(maxfd!=-1);
      int n=select(++maxfd,&input,NULL,NULL,(ms>0)?&tmo:NULL);

      // check for error
      if(n<0){
        ec==boost::asio::error::timed_out;
        return T{};
      }
      // check for tmo
      if(n==0){
        ec=boost::asio::error::timed_out;
        return T{};
      }
      // check if we got some data
      if(FD_ISSET(fd_,&input)){
        // NOTE! This is not correct
        // can we do it with a coroutine - build it by feeding bytes into the same place each time ...
        // if we use a stream, we may read too many characters which will be lost next time around deq() is called
        // maybe we van use boost::istream with a boost::iostreams::stream_buffer with a fixed lenth of exacly n bytes ...?

        // we only timeout the first time
        ms=0;
      }
    }
  }
  // queue state
  int fd_;                               // file descriptors to read from from
  DESER deser_;                          // de-serialiser
};
}
}
#endif

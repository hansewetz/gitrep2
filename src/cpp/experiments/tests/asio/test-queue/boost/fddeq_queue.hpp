/* NOTE!

- handle tmo correctly
- read more than one character at a time ... must then save chars after '\n' so we won;t miss any
- make message separator variable in ctor (default '\n')


*/

#ifndef __FDDEQ_QUEUE_H__
#define __FDDEQ_QUEUE_H__
#include "detail/queue_support.hpp"
#include <utility>
#include <iostream>
#include <sstream>
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
  // get file descriptor utility functions
  // (note: be carefulll when using - never close the fd unless you know no operation is in progress)
  int getfd()const{
    return fdread_;
  }
private:
  // deserialise an object from an fd stream
  T deserialize(std::size_t ms,boost::system::error_code&ec){
    T ret;                            // return value from this function (default ctor if no error)
    std::stringstream strstrm;        // collect read chars in a stringstream
    bool firsttime{true};             // track if this is the first time we call select
    while(true){
      // setup to listen on fd descriptor
      fd_set input;
      FD_ZERO(&input);
      FD_SET(fdread_,&input);
      int maxfd=fdread_;

      // setup for timeout
      struct timeval tmo;
      if(firsttime&&ms>0){
        // set timeout in select statement
        tmo.tv_sec=ms/1000;
        tmo.tv_usec=(ms%1000)*1000;
        firsttime=false;
      }
      // block on select - timeout if configured
      assert(maxfd!=-1);
      int n=select(++maxfd,&input,NULL,NULL,(ms>0)?&tmo:NULL);

      // check for error
      if(n<0){
        ec=boost::asio::error::timed_out;
        return T{};
      }
      // check for tmo
      if(n==0){
        ec=boost::asio::error::timed_out;
        return T{};
      }
      // check if we got some data
      if(FD_ISSET(fdread_,&input)){
        // read up to '\n' inclusive
        ssize_t count{0};
        while(true){
          char c;
          ssize_t stat;
          while((stat=::read(fdread_,&c,1))==EINTR){}
          if(stat!=1){
            // create a boost::system::error_code from errno
            ec=boost::system::error_code(errno,boost::system::get_posix_category());
            return T{};
          }
          if(c==NEWLINE){
            // don't include newline in deserialisation
            return deser_(strstrm);
          }
          // save character just read
          strstrm<<c;
        }
        // check if we read all available characters
        // (if there are no more chars to read, then restart select() statement)
        if(++count==n)break;
      }
    }
  }
  // queue state
  int fdread_;                           // file descriptors to read from from
  DESER deser_;                          // de-serialiser
  constexpr static int NEWLINE='\n';
};
}
}
#endif

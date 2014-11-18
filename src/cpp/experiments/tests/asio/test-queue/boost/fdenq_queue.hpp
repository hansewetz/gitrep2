/* NOTE!

IMPORTANT:
	- add abiity to cancel an operation

IMPROVEMENTS:
	- we should have two timeouts, message timeout, byte timeout
	- read more than one character at a time ... must then buffer what we have read
	- add option to include newline or not include it in sent message on receiver side, we should not include newline in message
	- maybe we should createa stream and serialise directoy into fd - we would have problems with tmos though ...

TESTING:
	- test with serializing real object and base64 encode them

*/

#ifndef __FDENQ_QUEUE_H__
#define __FDENQ_QUEUE_H__
#include "detail/queue_support.hpp"
#include <utility>
#include <unistd.h>

namespace boost{
namespace asio{

// a simple queue based on sending messages separated by '\n'
// (if sending objects first serialise the object, the base64 encode it in the serialiser)
template<typename T,typename SERIAL>
class fdenq_queue{
public:
  // default message separaor
  constexpr static char NEWLINE='\n';

  // typedef for value stored in queue
  using value_type=T;

  // ctors,assign,dtor
  fdenq_queue(int fdwrite,SERIAL serial,char sep=NEWLINE):fdwrite_(fdwrite),serial_(serial),sep_(sep){}
  fdenq_queue(fdenq_queue const&)=delete;
  fdenq_queue(fdenq_queue&&)=default;
  fdenq_queue&operator=(fdenq_queue const&)=delete;
  fdenq_queue&operator=(fdenq_queue&&)=default;
  ~fdenq_queue()=default;
  
  // enqueue a message (return.first == false if enq() was disabled)
  bool enq(T t,boost::system::error_code&ec){
    return this->send(t,0,ec,true);
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
  // get file descriptor utility functions
  int getfd()const{
    return fdwrite_;
  }
private:
  // serialise an object from an fd stream or wait until we timeout
  // (returns true we we could serialise object, false otherwise - error code will be non-zero if false)
  bool send(T const&t,std::size_t ms,boost::system::error_code&ec,bool sendMsg){
    std::stringstream strstrm;        // serialised object
    bool firsttime{true};             // track if this is the first time we call select

    // serialise object and get it as a string
    // (no need if we don;t need to send object)
    std::string str;
    if(sendMsg){
      serial_(strstrm,t);
      strstrm<<sep_;
      str=strstrm.str();
    }
    std::string::iterator sbegin{str.begin()};
    std::string::iterator send{str.end()};

    // loop until we have written message or timed out
    while(sbegin!=send){
      // setup to listen on fd descriptor
      fd_set output;
      FD_ZERO(&output);
      FD_SET(fdwrite_,&output);
      int maxfd=fdwrite_;

      // setup for timeout (ones we start writing a message we won't timeout)
      struct timeval tmo;
      if(firsttime&&ms>0){
        // set timeout in select statement
        tmo.tv_sec=ms/1000;
        tmo.tv_usec=(ms%1000)*1000;
        firsttime=false;
      }
      // block on select - timeout if configured
      assert(maxfd!=-1);
      int n=select(++maxfd,NULL,&output,NULL,(ms>0)?&tmo:NULL);

      // check for error
      if(n<0){
        ec=boost::system::error_code(errno,boost::system::get_posix_category());
        return false;
      }
      // check for tmo
      if(n==0){
        ec=boost::asio::error::timed_out;
        return false;
      }
      // check if we wrote some data
      if(FD_ISSET(fdwrite_,&output)){
        // if we are only checking if we can send a message
        if(!sendMsg){
          ec= boost::system::error_code{};
          return true;
        }
        // write as much as we can
        ssize_t count{0};
        while(sbegin!=send&&count!=n){
          // write next character in message
          char c{*sbegin++};
          ssize_t stat;
          while((stat=::write(fdwrite_,&c,1))==EINTR){}
          if(stat!=1){
            // create a boost::system::error_code from errno
            ec=boost::system::error_code(errno,boost::system::get_posix_category());
            return false;
          }
          ++count;
        }
      }
    }
  }
  // queue state
  int fdwrite_;                          // file descriptors serialize object tpo
  SERIAL serial_;                        // serialise
  char sep_;                              // message separator
};
}
}
#endif

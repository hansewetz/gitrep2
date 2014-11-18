/* NOTE!

TODO
	- we should have two timeouts
		- message arrival timeout
		- message reception timeout (#ms between bytes arriving while reading message)

IMPROVEMENTS:
	- add blocking/non-blocking calls for deq operations
	- read more than one character at a time ... must then buffer what we have read
	- add option to include newline or not include it in sent message on receiver side, we should not include newline in message

TESTING:
	- test with serializing real object and base64 encode them

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
// (the tmo in ms is based on message timeout - if no message starts arriving within timeout, the function times out)
// (ones we have started to read a message, the message will never timeout)
template<typename T,typename DESER>
class fddeq_queue{
public:
  // default message separaor
  constexpr static int NEWLINE='\n';

  // typedef for value stored in queue
  // (need this so we can create an item with default ctor)
  using value_type=T;

  // ctors,assign,dtor
  fddeq_queue(int fdread,DESER deser,int sep=NEWLINE):fdread_(fdread),deser_(deser),sep_{sep}{}
  fddeq_queue(fddeq_queue const&)=delete;
  fddeq_queue(fddeq_queue&&)=default;
  fddeq_queue&operator=(fddeq_queue const&)=delete;
  fddeq_queue&operator=(fddeq_queue&&)=default;
  ~fddeq_queue()=default;
  
  // dequeue a message (return.first == false if deq() was disabled)
  std::pair<bool,T>deq(boost::system::error_code&ec){
    T ret{deserialize(0,ec,true)};
    if(ec!=boost::system::error_code())return std::make_pair(false,ret);
    return make_pair(true,ret);
  }
  // dequeue a message (return.first == false if deq() was disabled) - timeout if waiting too long
  std::pair<bool,T>timed_deq(std::size_t ms,boost::system::error_code&ec){
    T ret{deserialize(ms,ec,true)};
    if(ec!=boost::system::error_code())return std::make_pair(false,ret);
    return make_pair(true,ret);
  }
  // wait until we can retrieve a message from queue
  bool wait_deq(boost::system::error_code&ec){
    deserialize(0,ec,false);
    if(ec.value()!=0)return false;
    return true;
  }
  // wait until we can retrieve a message from queue -  timeout if waiting too long
  bool timed_wait_deq(std::size_t ms,boost::system::error_code&ec){
    deserialize(ms,ec,false);
    if(ec==boost::asio::error::timed_out)return false;
    if(ec.value()!=0)return false;
    return true;
  }
  // get file descriptor utility functions
  // (note: be carefulll when using - never close the fd unless you know no operation is in progress)
  int getfd()const{
    return fdread_;
  }
private:
  // deserialise an object from an fd stream
  // or wait until there is a message to read - in this case, a default cibstructed object is returned
  T deserialize(std::size_t ms,boost::system::error_code&ec,bool getMsg){
    T ret;                            // return value from this function (default ctor if no error)
    std::stringstream strstrm;        // collect read chars in a stringstream
    bool firsttime{true};             // track if this is the first time we call select

    // loop until we have a message (or until we timeout)
    while(true){
      // setup to listen on fd descriptor
      fd_set input;
      FD_ZERO(&input);
      FD_SET(fdread_,&input);
      int maxfd=fdread_;

      // setup for timeout (ones we get a message we don't timeout)
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
        ec=boost::system::error_code(errno,boost::system::get_posix_category());
        return T{};
      }
      // check for tmo
      if(n==0){
        ec=boost::asio::error::timed_out;
        return T{};
      }
      // check if we got some data
      if(FD_ISSET(fdread_,&input)){
        // if we are only checking if we have a message we are done here
        if(!getMsg){
          ec= boost::system::error_code{};
          return T{};
        }
        // read up to '\n' inclusive
        ssize_t count{0};
        while(true){
          // read next character in message
          char c;
          ssize_t stat;
          while((stat=::read(fdread_,&c,1))==EINTR){}
          if(stat!=1){
            // create a boost::system::error_code from errno
            ec=boost::system::error_code(errno,boost::system::get_posix_category());
            return T{};
          }
          // save character just read
          strstrm<<c;

          // if we reached newline, send message including newline)
          if(c==sep_){
            return deser_(strstrm);
          }
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
  int sep_;                              // message separator
};
}
}
#endif

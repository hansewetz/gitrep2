// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

// NOTE! See site: http://www.tenouk.com/Module41.html

/* TODO

CANCELLATION: it's not clear how cancellation would be implemented - for right now I'll skip it

IMPROVEMENTS:
	- we should have two timeouts, message timeout, byte timeout
	- read more than one character at a time ... must then buffer what we have read

TESTING:
	- test with serializing real object and base64 encode them

*/

#ifndef __SOCK_CLIENT_QUEUE_H__
#define __SOCK_CLIENT_QUEUE_H__
#include "detail/queue_support.hpp"
#include "detail/fdqueue_support.hpp"
#include <string>
#include <utility>
#include <iostream>
#include <string>
#include <algorithm>
#include <string.h>

// socket stuff
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

// boost stuff
#include <boost/lexical_cast.hpp>

namespace boost{
namespace asio{

// a socket based client queue connecting to a server - full duplex queue - only 1 client can connect to the server
// (if sending objects first serialise the object, the base64 encode it in the serialiser)
// (the tmo in ms is based on message timeout - if no message starts arriving within timeout, the function times out)
// (ones we have started to read a message, the message will never timeout)
// (the class is meant to be used in singele threaded mode and is not thread safe)
template<typename T,typename DESER,typename SERIAL>
class sockclient_queue{
public:
  // typedef for value stored in queue
  using value_type=T;

  // default message separaor
  constexpr static char NEWLINE='\n';

  // ctors,assign,dtor
  sockclient_queue(std::string const&serverName,int port,DESER deser,SERIAL serial,char sep=NEWLINE):
      serverName_(serverName),port_(port),deser_(deser),serial_(serial),sep_{sep},closeOnExit_(true){
    // create socket
    createSocket();
  }
  sockclient_queue(sockclient_queue const&)=delete;
/*
  sockclient_queue(sockclient_queue&&other):
      deser_(std::move(other.deser_)),serial_(std::move(other.serial)),sep_(other.sep_),closeOnExit_(true),

      // server socket stuff
      port_(other.port_),servsocket_(other.servsocket_),yes_(other.yes_)
  {
    other.closeOnExit_=false; // make sure we don't close twice
    memcpy(static_cast<void*>(&servaddr_),static_cast<void*>(&other.servaddr_),sizeof(servaddr_));
  }
*/
  sockclient_queue&operator=(sockclient_queue const&)=delete;
/*
  sockclient_queue&operator=(sockclient_queue&&other){
    deser_=std::move(other.deser_);
    sep_=other.sep_;
    closeOnExit_=closeOnExit_;
    other.closeOnExit_=false; // make sure we don't close twice

    // server socket stuff
    port_=other.port_;
    servsocket_=other.servsocket_;
    memcpy(static_cast<void*>(&servaddr_),static_cast<void*>(&other.servaddr_),sizeof(servaddr_));
    yes_=other.yes_;
  }
*/
  ~sockclient_queue(){
    if(closeOnExit_){
      if(state_!=IDLE){
        if(state_==CONNECTED)detail::queue_support::eclose(servsocket_,false);
        detail::queue_support::eclose(servsocket_,false);
      }
    }
  }
  // dequeue a message (return.first == false if deq() was disabled)
  std::pair<bool,T>deq(boost::system::error_code&ec){
    return deqAux(0,ec,true);
  }
  // dequeue a message (return.first == false if deq() was disabled) - timeout if waiting too long
  std::pair<bool,T>timed_deq(std::size_t ms,boost::system::error_code&ec){
    return deqAux(ms,ec,true);
  }
  // wait until we can retrieve a message from queue
  bool wait_deq(boost::system::error_code&ec){
    deqAux(0,ec,false);
    if(ec.value()!=0)return false;
    return true;
  }
  // wait until we can retrieve a message from queue -  timeout if waiting too long
  bool timed_wait_deq(std::size_t ms,boost::system::error_code&ec){
    deqAux(ms,ec,false);
    if(ec==boost::asio::error::timed_out)return false;
    if(ec.value()!=0)return false;
    return true;
  }

 // NOTE! Not yet implkemented
  // enqueue a message (return.first == false if enq() was disabled)
  bool enq(T t,boost::system::error_code&ec){
    // NOTE! Not yet implemented
  }
  // enqueue a message (return.first == false if enq() was disabled) - timeout if waiting too long
  bool timed_enq(T t,std::size_t ms,boost::system::error_code&ec){
    // NOTE! Not yet implemented
  }
  // wait until we can retrieve a message from queue
  bool wait_enq(boost::system::error_code&ec){
    // NOTE! Not yet implemented
  }
  // wait until we can retrieve a message from queue -  timeout if waiting too long
  bool timed_wait_enq(std::size_t ms,boost::system::error_code&ec){
    // NOTE! Not yet implemented
  }
 // NOTE! Not yet implkemented
  
private:
  // --------------------------------- state management functions
  // (all state is managed here)
  // dequeue a message (return.first == false if deq() was disabled)
  std::pair<bool,T>deqAux(std::size_t ms,boost::system::error_code&ec,bool getMsg){
    boost::system::error_code ec1;

    // if we are in IDLE state it means we are disconnected
    if(state_==IDLE){
      connect2server(ec1);
      if(ec1!=boost::system::error_code()){
        detail::queue_support::eclose(servsocket_,false);
        ec=ec1;
        return std::make_pair(false,T{});
      }
      state_=CONNECTED;
    }
    // client connected - read message
    if(state_==CONNECTED){
      state_=READING;
      T ret{detail::queue_support::recvwait<T,DESER>(servsocket_,ms,ec1,getMsg,sep_,deser_)};
      if(ec1!=boost::system::error_code()){
        detail::queue_support::eclose(servsocket_,false);
        state_=IDLE;
        ec=ec1;
        return std::make_pair(false,T{});
      }
      state_=CONNECTED;
      return make_pair(true,ret);
    }
  }
  // --------------------------------- helper functions
  // (no state is managed here)
  // create listen socket (throws exception if failure)
  void createSocket(){
    // we must be in state IDLE to be here
    assert(state_==IDLE);

    // get socket to listen on
    if((servsocket_=socket(AF_INET,SOCK_STREAM,0))==-1){
      throw std::runtime_error(std::string("sockclient_queue::connect2Server: failed creating socket, errno: ")+boost::lexical_cast<std::string>(errno));
    }
    // set socket options for listning socket
    if(setsockopt(servsocket_,SOL_SOCKET,SO_REUSEADDR,&yes_,sizeof(int))==-1){
      throw std::runtime_error(std::string("sockclient_queue::connect2Server: failed setting socket options, errno: ")+boost::lexical_cast<std::string>(errno));
    }
    // get server host
    server_=gethostbyname(serverName_.c_str());
    if (server_==NULL) {
      throw std::runtime_error(std::string("sockclient_queue::connect2Server: failed converting ")+
                               serverName_+" to hostname, errno: "+boost::lexical_cast<std::string>(errno));
    }
    // setup addresses so we can connect later
    bzero((char *)&servaddr_,sizeof(servaddr_));
    servaddr_.sin_family=AF_INET;
    bcopy((char *)server_->h_addr,(char*)&servaddr_.sin_addr.s_addr,server_->h_length);
    servaddr_.sin_port=htons(port_);
  }
  // connect to server
  void connect2server(boost::system::error_code&ec){
    // connect to server
    ec=boost::system::error_code();
    if(connect(servsocket_,(const struct sockaddr*)&servaddr_,sizeof(servaddr_))<0) {
      ec=boost::system::error_code(errno,boost::system::get_posix_category());
    }
    state_=CONNECTED;
  }
  // --------------------------------- private attributes
  // state object is in
  enum state_t{IDLE=0,CONNECTED=1,READING=2,WRITING=3};
  state_t state_=IDLE;

  // state of queue
  DESER deser_;                          // de-serialiser
  SERIAL serial_;                        // serialiser
  char sep_;                             // message separator
  bool closeOnExit_;                     // close fd on exit (if we have been moved we don;t close)
  std::string serverName_;               // server to connect to as a name

  // server socket stuff
  struct hostent*server_;                // server when converting name to host
  int port_;                             // port to listen on
  int servsocket_=-1;                    // socket used by client
  struct sockaddr_in servaddr_;          // server address
  int yes_=1;                            // used for setting socket options
};
}
}
#endif

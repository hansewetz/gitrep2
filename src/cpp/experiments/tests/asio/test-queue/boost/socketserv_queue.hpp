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

#ifndef __SOCK_SERV_QUEUE_H__
#define __SOCK_SERV_QUEUE_H__
#include "detail/queue_support.hpp"
#include <string>
#include <utility>
#include <iostream>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <string.h>

// socket stuff
#include <sys/types.h>
#include <sys/socket.h>

// boost stuff
#include <boost/lexical_cast.hpp>

namespace boost{
namespace asio{

// a socket based server queue listening on clients - full duplex queue - only 1 client can connect
// (if sending objects first serialise the object, the base64 encode it in the serialiser)
// (the tmo in ms is based on message timeout - if no message starts arriving within timeout, the function times out)
// (ones we have started to read a message, the message will never timeout)
// (the class is meant to be used in singele threaded mode and is not thread safe)
template<typename T,typename DESER,typename SERIAL>
class sockserv_queue{
public:
  // typedef for value stored in queue
  using value_type=T;

  // default message separaor
  constexpr static char NEWLINE='\n';

  // ctors,assign,dtor
  sockserv_queue(int port,DESER deser,SERIAL serial,char sep=NEWLINE):
      port_(port),deser_(deser),serial_(serial),sep_{sep},closeOnExit_(true){
    // start listening on socket
    createListenSocket();
  }
  sockserv_queue(sockserv_queue const&)=delete;
  sockserv_queue(sockserv_queue&&other):
      deser_(std::move(other.deser_)),serial_(std::move(other.serial)),sep_(other.sep_),closeOnExit_(true),

      // server socket stuff
      port_(other.port_),servsocket_(other.servsocket_),yes_(other.yes_)
  {
    other.closeOnExit_=false; // make sure we don't close twice
    memcpy(static_cast<void*>(&serveraddr_),static_cast<void*>(&other.serveraddr_),sizeof(serveraddr_));
  }
  sockserv_queue&operator=(sockserv_queue const&)=delete;
  sockserv_queue&operator=(sockserv_queue&&other){
    deser_=std::move(other.deser_);
    sep_=other.sep_;
    closeOnExit_=closeOnExit_;
    other.closeOnExit_=false; // make sure we don't close twice

    // server socket stuff
    port_=other.port_;
    servsocket_=other.servsocket_;
    memcpy(static_cast<void*>(&serveraddr_),static_cast<void*>(&other.serveraddr_),sizeof(serveraddr_));
    yes_=other.yes_;
  }
  ~sockserv_queue(){
    if(closeOnExit_){
      if(state_!=IDLE){
        if(state_==CONNECTED)detail::queue_support::eclose(clientsocket_,false);
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
    deqAux(0,ec,true);
    if(ec.value()!=0)return false;
    return true;
  }
  // wait until we can retrieve a message from queue -  timeout if waiting too long
  bool timed_wait_deq(std::size_t ms,boost::system::error_code&ec){
    deqAux(ms,ec,true);
    if(ec==boost::asio::error::timed_out)return false;
    if(ec.value()!=0)return false;
    return true;
  }

  // NOTE! Not yet done
  // ...
  
private:
  // --------------------------------- state management functions
  // (all state is managed here)

  // dequeue a message (return.first == false if deq() was disabled)
  std::pair<bool,T>deqAux(std::size_t ms,boost::system::error_code&ec,bool getMsg){
    boost::system::error_code ec1;

    // wait for client connection if needed
    if(state_==IDLE){
      waitForClientConnect(ms,ec1);
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
      T ret{recvwait(0,ec1,getMsg)};
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
  void createListenSocket(){
    // we must be in state IDLE to be here
    assert(state_==IDLE);

    // get socket to listen on
    if((servsocket_=socket(AF_INET,SOCK_STREAM,0))==-1){
      throw std::runtime_error(std::string("sockserv_queue::createListenSocket: failed creating listening socket, errno: ")+boost::lexical_cast<std::string>(errno));
    }
    // set socket options for listning socket
    if(setsockopt(servsocket_,SOL_SOCKET,SO_REUSEADDR,&yes_,sizeof(int))==-1){
      throw std::runtime_error(std::string("sockserv_queue::createListenSocket: failed setting socket options, errno: ")+boost::lexical_cast<std::string>(errno));
    }
    // bind adddr/socket
    serveraddr_.sin_family=AF_INET;
    serveraddr_.sin_addr.s_addr=INADDR_ANY;
    serveraddr_.sin_port=htons(port_);
    memset(&(serveraddr_.sin_zero),'\0',8);
    if(bind(servsocket_,(struct sockaddr*)&serveraddr_,sizeof(serveraddr_))==-1){
      throw std::runtime_error(std::string("sockserv_queue::createListenSocket: failed binding socket to address, errno: ")+boost::lexical_cast<std::string>(errno));
    }
    // start listening on soclet
    if(listen(servsocket_,maxclients_)==-1){
      throw std::runtime_error(std::string("sockserv_queue::createListenSocket: failed listening on socket, errno: ")+boost::lexical_cast<std::string>(errno));
    }
  }
  // wait until a client connects and acept connection
  // (if ms == 0, no timeout, we must be in state IDLE when being called0
  void waitForClientConnect(std::size_t ms,boost::system::error_code&ec){
    // we must be in state IDLE to be here
    assert(state_==IDLE);

    // setup to listen on fd descriptor
    fd_set input;
    FD_ZERO(&input);
    FD_SET(servsocket_,&input);
    int maxfd=servsocket_;

    // setup for timeout (ones we get a message we don't timeout)
    struct timeval tmo;
    tmo.tv_sec=ms/1000;
     tmo.tv_usec=(ms%1000)*1000;
      
    // block on select - timeout if configured
    assert(maxfd!=-1);
    int n=::select(++maxfd,&input,NULL,NULL,ms>0?&tmo:NULL);

    // error
    if(n<0){
      ec=boost::system::error_code(errno,boost::system::get_posix_category());
      return;
    }
    // tmo
    if(n==0){
      ec=boost::asio::error::timed_out;
      return;
    }
    // client connected
    unsigned int addrlen;
    if((clientsocket_=::accept(servsocket_,(struct sockaddr*)&clientaddr_,&addrlen)) == -1){
      ec=boost::system::error_code(errno,boost::system::get_posix_category());
      return;
    }
    // no errors - client is now connected
    ec=boost::system::error_code();
  }
  // read a message from client socket
  // or wait until there is a message to read - in this case, a default cibstructed object is returned
  T recvwait(std::size_t ms,boost::system::error_code&ec,bool getMsg){
    T ret;                            // return value from this function (default ctor if no error)
    std::stringstream strstrm;        // collect read chars in a stringstream

    // we must be in state CONNECTED to be here
    assert(state_==CONNECTED);

    // loop until we have a message (or until we timeout)
    while(true){
      // setup to listen on fd descriptor
      fd_set input;
      FD_ZERO(&input);
      FD_SET(servsocket_,&input);
      FD_SET(clientsocket_,&input);
      int maxfd=std::max(servsocket_,clientsocket_);

      // setup for timeout (ones we get a message we don't timeout)
      struct timeval tmo;
      tmo.tv_sec=ms/1000;
      tmo.tv_usec=(ms%1000)*1000;
      
      // block on select - timeout if configured
      assert(maxfd!=-1);
      int n=::select(++maxfd,&input,NULL,NULL,ms>0?&tmo:NULL);

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
      if(FD_ISSET(clientsocket_,&input)){
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
          while((stat=::read(clientsocket_,&c,1))==EINTR){}
          if(stat!=1){
            // create a boost::system::error_code from errno
            ec=boost::system::error_code(errno,boost::system::get_posix_category());
            return T{};
          }
          // save character just read
          strstrm<<c;

          // if we reached newline, send message including newline)
          if(c==sep_)return deser_(strstrm);
        }
        // check if we read all available characters
        // (if there are no more chars to read, then restart select() statement)
        if(++count==n)break;
      }
      // restet tmo 0 zero ms since we don't timeout ones we start reading a message
      ms=0;
    }
  }


  // --------------------------------- private attributes
  // state object is in
  enum state_t{IDLE=0,CONNECTED=1,READING=2,WRITING=3};
  state_t state_=IDLE;

  // state of queue
  DESER deser_;                          // de-serialiser
  SERIAL serial_;                        // serialiser
  std::size_t maxclients_=1;             // max clients that can connect to this queue - always equal to 1
  char sep_;                             // message separator
  bool closeOnExit_;                     // close fd on exit (if we have been moved we don;t close)

  // server socket stuff
  int port_;                             // port to listen on
  int servsocket_;                       // socket on which we are listening
  struct sockaddr_in serveraddr_;        // server address
  int yes_=1;                            // used for setting socket options

  // client stuff
  int clientsocket_=-1;                  // socket for client
  struct sockaddr_in clientaddr_;        // client address
};
}
}
#endif

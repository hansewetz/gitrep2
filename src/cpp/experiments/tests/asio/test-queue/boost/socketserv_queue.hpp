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
#include <string>
#include <utility>
#include <iostream>
#include <string>
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
      // NOTE! Not yet done yet
      // ...
    }
  }

  // NOTE! Not yet done
  // ...
  
private:
  // create listen socket (throws exception if failure)
  void createListenSocket(){
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
      int addrlen;
      if((clientsocket_=::accept(servsocket_,(struct sockaddr*)&clientaddr_,&addrlen)) == -1){
        ec=boost::system::error_code(errno,boost::system::get_posix_category());
        return;
      }
      // no errors - client is now connected
      state_=CONNECTED;
      ec=boost::system::error_code();
  }



  // state object is in
  enum state_t{IDLE=0,CONNECTED=1,READING=2,WRITING=3,ERROR=4};
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

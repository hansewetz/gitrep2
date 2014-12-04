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

// a socket based server queue listening on clients - full duplex queue
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
  sockserv_queue(int port,DESER deser,SERIAL serial,std::size_t maxclients,char sep=NEWLINE):
      port_(port),deser_(deser),serial_(serial),maxclients_(maxclients),sep_{sep},closeOnExit_(true){
    // start listening on socket
    createListenSocket();
  }
  sockserv_queue(sockserv_queue const&)=delete;
  sockserv_queue(sockserv_queue&&other):
      deser_(std::move(other.deser_)),serial_(std::move(other.serial)),maxclients_(other.maxclients_),sep_(other.sep_),closeOnExit_(true),

      // server socket stuff
      port_(other.port_),listenSocket_(other.listenSocket_),yes_(other.yes_)
  {
    other.closeOnExit_=false; // make sure we don't close twice
    memcpy(static_cast<void*>(&serveraddr_),static_cast<void*>(&other.serveraddr_),sizeof(serveraddr_));
  }
  sockserv_queue&operator=(sockserv_queue const&)=delete;
  sockserv_queue&operator=(sockserv_queue&&other){
    deser_=std::move(other.deser_);
    maxclients_=other.maxclients_;
    sep_=other.sep_;
    closeOnExit_=closeOnExit_;
    other.closeOnExit_=false; // make sure we don't close twice

    // server socket stuff
    port_=other.port_;
    listenSocket_=other.listenSocket_;
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
    if((listenSocket_=socket(AF_INET,SOCK_STREAM,0))==-1){
      throw std::runtime_error(std::string("sockserv_queue::createListenSocket: failed creating listening socket, errno: ")+boost::lexical_cast<std::string>(errno));
    }
    // set socket options for listning socket
    if(setsockopt(listenSocket_,SOL_SOCKET,SO_REUSEADDR,&yes_,sizeof(int))==-1){
      throw std::runtime_error(std::string("sockserv_queue::createListenSocket: failed setting socket options, errno: ")+boost::lexical_cast<std::string>(errno));
    }
    // bind adddr/socket
    serveraddr_.sin_family=AF_INET;
    serveraddr_.sin_addr.s_addr=INADDR_ANY;
    serveraddr_.sin_port=htons(port_);
    memset(&(serveraddr_.sin_zero),'\0',8);
    if(bind(listenSocket_,(struct sockaddr*)&serveraddr_,sizeof(serveraddr_))==-1){
      throw std::runtime_error(std::string("sockserv_queue::createListenSocket: failed binding socket to address, errno: ")+boost::lexical_cast<std::string>(errno));
    }
    // start listening on soclet
    if(listen(listenSocket_,maxclients_)==-1){
      throw std::runtime_error(std::string("sockserv_queue::createListenSocket: failed listening on socket, errno: ")+boost::lexical_cast<std::string>(errno));
    }
  }


  // state of queue
  DESER deser_;                          // de-serialiser
  SERIAL serial_;                        // serialiser
  std::size_t maxclients_;               // max clients that can connect to this queue
  char sep_;                             // message separator
  bool closeOnExit_;                     // close fd on exit (if we have been moved we don;t close)

  // server socket stuff
  int port_;                             // port to listen on
  int listenSocket_;                     // socket on which we are listening
  struct sockaddr_in serveraddr_;        // server address
  int yes_=1;                            // used for setting socket options
};
}
}
#endif

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#ifndef __FSOCK_QUEUE_SUPPPORT_H__
#define __FSOCK_QUEUE_SUPPPORT_H__
#include <utility>
#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/select.h>
#include <boost/asio/error.hpp>

// socket stuff
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

// boost stuff
#include <boost/lexical_cast.hpp>

namespace boost{
namespace asio{
namespace detail{
namespace sockqueue_support{

namespace{

// create listen socket (throws exception if failure)
// (returns server socket)
int createListenSocket(int port,struct sockaddr_in&serveraddr,std::size_t maxclients,int*yes){
  // get socket to listen on
  int ret{-1};
  if((ret=socket(AF_INET,SOCK_STREAM,0))==-1){
    throw std::runtime_error(std::string("createListenSocket: failed creating listening socket, errno: ")+boost::lexical_cast<std::string>(errno));
  }
  // set socket options for listning socket
  if(setsockopt(ret,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){
    throw std::runtime_error(std::string("createListenSocket: failed setting socket options, errno: ")+boost::lexical_cast<std::string>(errno));
  }
  // bind adddr/socket
  serveraddr.sin_family=AF_INET;
  serveraddr.sin_addr.s_addr=INADDR_ANY;
  serveraddr.sin_port=htons(port);
  memset(&(serveraddr.sin_zero),'\0',8);
  if(bind(ret,(struct sockaddr*)&serveraddr,sizeof(serveraddr))==-1){
    throw std::runtime_error(std::string("createListenSocket: failed binding socket to address, errno: ")+boost::lexical_cast<std::string>(errno));
  }
  // start listening on socket
  if(listen(ret,maxclients)==-1){
    throw std::runtime_error(std::string("createListenSocket: failed listening on socket, errno: ")+boost::lexical_cast<std::string>(errno));
  }
  // return server socket
  return ret;
}
// wait until a client connects and acept connection
// (if ms == 0, no timeout, we must be in state IDLE when being called0
int waitForClientConnect(int servsocket,struct sockaddr_in&serveraddr,struct sockaddr_in&clientaddr,std::size_t ms,boost::system::error_code&ec){
  int ret{-1};

  // setup to listen on fd descriptor
  fd_set input;
  FD_ZERO(&input);
  FD_SET(servsocket,&input);
  int maxfd=servsocket;

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
    return ret;
  }
  // tmo
  if(n==0){
    ec=boost::asio::error::timed_out;
    return ret;
  }
  // client connected
  unsigned int addrlen;
  if((ret=::accept(servsocket,(struct sockaddr*)&clientaddr,&addrlen)) == -1){
    ec=boost::system::error_code(errno,boost::system::get_posix_category());
    return ret;
  }
  // no errors - client is now connected, return client socket
  ec=boost::system::error_code();
  return ret;
}
// NOTE! Not yet done
// read data and accept client connections in a select loop
template<typename T,typename DESER>
void acceptClientsAndDequeue(int servsocket){
  // loop until we have a message (or until we timeout)
  while(true){
    // setup to listen on fd descriptor
    fd_set input;
    FD_ZERO(&input);
    FD_SET(servsocket,&input);
    int maxfd=servsocket;

    // setup for timeout (ones we get a message we don't timeout)
    // NOTE! Only for debug purpose
    std::size_t ms{3000};
    struct timeval tmo;
    tmo.tv_sec=ms/1000;
    tmo.tv_usec=(ms%1000)*1000;
    
    // block on select - timeout if configured
    assert(maxfd!=-1);
    int n=::select(++maxfd,&input,NULL,NULL,&tmo);

    // check for error
    if(n<0){
      // NOTE!
      std::cout<<"<ERROR>: "<<std::strerror(errno)<<std::endl;
      continue;
    }
    // check for tmo
    if(n==0){
      // NOTE!
      std::cout<<"<HEARTBEAT>"<<std::endl;
      continue;
    }
    // check for client connecting
    if(FD_ISSET(servsocket,&input)){
      // NOTE! Not yet done
      std::cout<<"<CLIENT-CONNECT>"<<std::endl;
    }
    // check for data on existing client connection
    // NOTE! Not yet deon
  }
}
}
}
}
}
}
#endif

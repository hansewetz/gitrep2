// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#ifndef __FSOCK_QUEUE_SUPPPORT_H__
#define __FSOCK_QUEUE_SUPPPORT_H__
// support functions
#include "sockqueue_support.hpp"

// NOTE!
#include <algorithm>
#include <iterator>

// standard and boost stuff
#include <atomic>
#include <utility>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
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
void acceptClientsAndDequeue(int servsocket,char sep,std::atomic<bool>&stop_server){
  // data structures tracking client fds and correpsonding data
  std::unordered_map<int,std::vector<char>>client_data;

  // loop until server 'stop_server' flag is set
  while(!stop_server.load()){
    // setup to listen on server descriptor
    fd_set input;
    FD_ZERO(&input);
    FD_SET(servsocket,&input);
    int maxfd=servsocket;

    // setup to listen on client fds
    for(auto const&p:client_data){
      FD_SET(p.first,&input);
      maxfd=std::max(maxfd,p.first);
    }
    // setup for timeout (ones we get a message we don't timeout)
    // NOTE! Only for debug purpose
    std::size_t ms{1000};
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

      // accept client connection
      // (if failure - continue)
      unsigned int addrlen;
      struct sockaddr_in clientaddr;
      int client_fd;
      if((client_fd=::accept(servsocket,(struct sockaddr*)&clientaddr,&addrlen))==-1){
        // NOTE!
        std::cout<<"<ERROR>: accept: "<<std::strerror(errno)<<std::endl;
        continue;
      }
      // set client fd to non blocking
      auto err=detail::queue_support::setFdNonblock(client_fd);
      if(err!=0){
        // NOTE!
        std::cout<<"<ERROR>: non-block: "<<std::strerror(errno)<<std::endl;
        detail::queue_support::eclose(client_fd,false);
        continue;
      }
      // add client connection to active clients
      // NOTE!
      std::cout<<"<CLIENT-ACCEPT> client_fd: "<<client_fd<<std::endl;
      client_data.insert(std::make_pair(client_fd,std::vector<char>{}));
    }
    // check for data on existing client connection
    for(auto&p:client_data){
      int client_fd(p.first);
      auto&buf(p.second);

      // if no activity on this fd, skip it
      if(!FD_ISSET(client_fd,&input))continue;

      // read as much as we can from client_fd
      // (create objects as we read)
      bool firstTime{true};
      while(true){
        // read next character in message
        char c;
        ssize_t stat;
        while((stat=::read(client_fd,&c,1))==EINTR){}

        // check if there is nothing more to read
        // (if we have already read characters and we get a read error we'll continue to track the client_fd)
        if(!firstTime&&stat!=1){
          break;
        }
        // we lost client connection
        if(stat!=1){
          // we have a real read error - remove client fd and close it
          // (no need to check for EWOULDBLOCK since we'll never block on client_fd)
          // NOTE!
          std::cout<<"<CLIENT-DISCONNECT> client_fd: "<<client_fd<<std::endl;
          detail::queue_support::eclose(client_fd,false);
          client_data.erase(client_fd);
          break;
        }
        // save character just read
        buf.push_back(c);
        firstTime=false;

        // if we reached sep, send message including newline)
        if(c==sep){
          // NOTE! deserialise data, insert object into container and clear buffer for fd_client
          std::copy(buf.begin(),buf.end(),std::ostream_iterator<char>(std::cout));

          // clear buffer
          buf.clear();
        }
      }
    }
  }
  // close all client fds
  // (collected data will be destroyed automatically)
  for(auto const&p:client_data){
    detail::queue_support::eclose(p.first,false);
  }
}
}
}
}
}
}
#endif

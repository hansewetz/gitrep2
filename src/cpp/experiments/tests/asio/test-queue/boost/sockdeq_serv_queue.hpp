// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

/* TODO:
	- setup a clear line where attributes are used by both select and interface side
	- design an interupt mechanism for rge select side (tmo+check done, an fd-pipe which select side listens on, ...)
	- potentially disable queue - not used disable dequeue
*/

#ifndef __SOCK_DEQ_SERV_QUEU_H__
#define __SOCK_DEQ_SERV_QUEU_H__
#include "detail/queue_empty_base.hpp"
#include "detail/sockqueue_support.hpp"
#include <boost/asio/error.hpp>
#include <string>
#include <utility>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
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

template<typename T,typename DESER,typename Base=detail::base::queue_empty_base<T>,typename Container=std::queue<T>>
class sockdeq_serv_queue:public Base{
public:
  // default message separaor
  constexpr static char NEWLINE='\n';

  // ctor
  sockdeq_serv_queue(int port,DESER deser,std::size_t maxclients,char sep=NEWLINE):
      port_(port),deser_(deser),maxclients_(maxclients),sep_{sep},closeOnExit_(true),
      mtx_{std::make_unique<std::mutex>()},cond_{std::make_unique<std::condition_variable>()}{
    // spawn thread running socket io stuff
    thr_=std::move(std::thread([&](){run_sock_io();}));
  }
  // copy ctor
  sockdeq_serv_queue(sockdeq_serv_queue const&)=delete;

  // move ctor
  sockdeq_serv_queue(sockdeq_serv_queue&&other):
      port_(other.port_),deser_(std::move(other.deser_)),maxclients_(other.maxclients_),sep_(other.sep_),closeOnExit_(other.closeOnExit_),
      mtx_(std::move(other.mtx_)),cond_(other.cond_),
      deq_enabled_(other.deq_enabled_)
  {
    other.closeOnExit_=false; // make sure we don't close twice
    memcpy(static_cast<void*>(&serveraddr_),static_cast<void*>(&other.serveraddr_),sizeof(serveraddr_));
  }
  // assign
  sockdeq_serv_queue&operator=(sockdeq_serv_queue const&)=delete;

  // move assign
  sockdeq_serv_queue&operator=(sockdeq_serv_queue&&other){
    port_=other.port_;
    deser_=std::move(other.deser_);
    maxclients_=other.maxclients_;
    sep_=other.sep_;
    closeOnExit_=other.closeOnExit_;
    other.closeOnExit_=false; // make sure we don't close twice

    // server socket stuff
    servsocket_=other.servsocket_;
    yes_=other.yes_;
    memcpy(static_cast<void*>(&serveraddr_),static_cast<void*>(&other.serveraddr_),sizeof(serveraddr_));

    mtx_=std::move(other.mtx_);
    cond_=std::move(other.cond_);
    deq_enabled_=other.deq_enabled_;
    return*this;
  }
  // ctor
  ~sockdeq_serv_queue(){
// NOTE! Must stop select loop ...
// Not yet done

    if(closeOnExit_){
      // NOTE! Must check if socket is open
      detail::queue_support::eclose(servsocket_,false);
    }
    // join socket io thread
    thr_.join();
  }

  // NOTE! Not yet done

private:
  // --------------------------------- private helper functions
  void run_sock_io(){
    // creating a listening socket (server socket)
    servsocket_=detail::sockqueue_support::createListenSocket(port_,serveraddr_,maxclients_,&yes_);

    // accept client connections and dequeue messages
    // NOTE! Not correct
    detail::sockqueue_support::acceptClientsAndDequeue<T,DESER>(servsocket_);
  }
  // --------------------------------- private data
  // user specified state for queue
  int port_;                             // port to listen on
  DESER const deser_;                    // de-serialiser
  std::size_t const maxclients_;         // max clients that can connect to this queue
  char const sep_;                       // message separator
  bool const closeOnExit_;               // close fd on exit

  // thread handling socket io stuff
  std::thread thr_;

  // server socket stuff
  int servsocket_;                       // socket on which we are listening
  struct sockaddr_in serveraddr_;        // server address
  int yes_=1;                            // used for setting socket options

  // mutex and cond variable managing queue of messages
  mutable std::unique_ptr<std::mutex>mtx_;               // must be pointer since not movable
  mutable std::unique_ptr<std::condition_variable>cond_; // must be pointer since not movable
  bool deq_enabled_=true;                                // is dequing enabled

};
}
}
#endif

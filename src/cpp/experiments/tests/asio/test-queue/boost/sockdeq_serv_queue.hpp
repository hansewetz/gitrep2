// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

/* TODO:
	- setup a clear line where attributes are used by both select and interface side
	- design an interupt mechanism for select side (tmo+check done, an fd-pipe which select side listens on, ...)
	- potentially disable queue - not used disable dequeue

HACKS:
------
	- we set a flag to tell server loop to stop - that is, we must have a timer going in server which checks the flag each time the timer pops
	- must protect q_ when moving an object
	- we must have better recovery mechanisms
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
#include <atomic>
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
  sockdeq_serv_queue(int port,DESER deser,std::size_t maxclients,std::size_t tmo_poll_ms,char sep=NEWLINE):
      port_(port),deser_(deser),maxclients_(maxclients),tmo_poll_ms_(tmo_poll_ms),sep_{sep},
      mtx_{std::make_unique<std::mutex>()},cond_{std::make_unique<std::condition_variable>()},stop_server_(false){
    // spawn thread running socket io stuff
    serv_thr_=std::move(std::thread([&](){run_sock_serv();}));
  }
  // copy/move/assign - for simplicity, delete them
  sockdeq_serv_queue(sockdeq_serv_queue const&)=delete;
  sockdeq_serv_queue(sockdeq_serv_queue&&other)=delete;
  sockdeq_serv_queue&operator=(sockdeq_serv_queue const&)=delete;
  sockdeq_serv_queue&operator=(sockdeq_serv_queue&&other)=delete;

  // ctor
  ~sockdeq_serv_queue(){
    // flag to stop server loop and wait for server to stop, then close server socket
    stop_server_.store(true);
    if(serv_thr_.joinable())serv_thr_.join();
    detail::queue_support::eclose(servsocket_,false);
  }
  // dequeue a message
  std::pair<bool,T>deq(boost::system::error_code&ec){
    std::unique_lock<std::mutex>lock(*mtx_);
    cond_->wait(lock,[&](){return !deq_enabled_||!q_.empty();});
  
    // if deq is disabled or timeout
    if(!deq_enabled_){
      ec=boost::asio::error::operation_aborted;
      return std::make_pair(false,T{});
    }
    // check if we have a message
    std::pair<bool,T>ret{std::make_pair(true,q_.front())};
    q_.pop();
    cond_->notify_all();
    ec=boost::system::error_code();
    return ret;
  }

  // NOTE! Not yet done

private:
  // --------------------------------- private helper functions

  // callback function creating an object from an istream
  void createItem(std::istream&is){
    // NOTE! Not yet done
    std::string line;
    std::getline(is,line);
    std::cout<<line<<std::endl;
  }
  // function running select loop
  void run_sock_serv(){
    // create listening socket (server socket)
    servsocket_=detail::sockqueue_support::createListenSocket(port_,serveraddr_,maxclients_,&yes_);

    // accept client connections and dequeue messages
    detail::sockqueue_support::acceptClientsAndDequeue(servsocket_,sep_,tmo_poll_ms_,stop_server_,[&](std::istream&is){createItem(is);});
  }
  // --------------------------------- private data
  // user specified state for queue
  int port_;                             // port to listen on
  DESER const deser_;                    // de-serialiser
  std::size_t const maxclients_;         // max clients that can connect to this queue
  std::size_t const tmo_poll_ms_;        // ms poll intervall for checking if queue should be stopped
  char const sep_;                       // message separator

  // state of interface to queue
  bool deq_enabled_=true;                // is dequing enabled

  // server socket stuff
  std::thread serv_thr_;                 // thread handling dequeing messages
  int servsocket_;                       // socket on which we are listening
  struct sockaddr_in serveraddr_;        // server address
  int yes_=1;                            // used for setting socket options

  // variables shared across select loop and interface
  Container q_;                                          // queues waiting to be de-queued
  mutable std::unique_ptr<std::mutex>mtx_;               // protects queue where messages are stored
  mutable std::unique_ptr<std::condition_variable>cond_; // ...
  std::atomic<bool>stop_server_;                         // when set to true, then server will stop
};
}
}
#endif

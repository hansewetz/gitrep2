/*
  queue runs a thread which runs asio::dir_monitor
  at startup the queue reads all queue items (files) in the queue directory
  when the directory has a file created or removed it updates the cache in the queue
  both ipc mutices/condition-variables will be needed (when asio callback updates the cache the main queue thread needs to be notified
  not clear of if we should use internal mutex/cond-variables for internal notification in this queue
*/
#ifndef __EVENTDIR_QUEUE_H__
#define __EVENTDIR_QUEUE_H__
#include "detail/dirqueue_support.h"
#include <string>
#include <utility>
#include <list>
#include <boost/filesystem.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>

namespace boost{
namespace asio{

namespace fs=boost::filesystem;
namespace ipc=boost::interprocess;
namespace pt= boost::posix_time;

template<typename T,typename DESER,typename SERIAL>
class eventdir_queue{
public:
  // typedef for value stored in queue
  // (need this so we can create an item with default ctor)
  using value_type=T;

  // typedef for serialiser/de-serialiser
  using deserialiser=DESER;
  using serialiser=SERIAL;

  // ctors,assign,dtor
  eventdir_queue(std::size_t maxsize,fs::path const&dir,DESER deser,SERIAL serial,bool removelocks):
      maxsize_(maxsize),dir_(dir),deser_(deser),serial_(serial),removelocks_(removelocks),
      ipcmtx_(ipc::open_or_create,detail::dirqueue_support::getMutexName(dir).c_str()),ipccond_(ipc::open_or_create,detail::dirqueue_support::getCondName(dir).c_str()){
    // make sure path is a directory
    if(!fs::is_directory(dir_))throw std::logic_error(std::string("eventdir_queue::eventdir_queue: dir_: ")+dir.string()+" is not a directory");

    // lock directory and read content
    // ...

    // setup to receive events from inotify
    // ...
  }
  eventdir_queue(eventdir_queue const&)=delete;
  eventdir_queue(eventdir_queue&&)=default;
  eventdir_queue&operator=(eventdir_queue const&)=delete;
  eventdir_queue&operator=(eventdir_queue&&)=default;
  ~eventdir_queue(){
    if(removelocks_)removeLockVariables(dir_);
  }
  // put a message into queue
  // (returns true if message was enqueued, false if enqueing was disabled)
  bool enq(T t){
    // NOTE! Not yet done
  }
  // wait until we can put a message in queue
  // (returns false if enqueing was disabled, else true)
  bool wait_enq(){
    // NOTE! Not yet done
  }
  // dequeue a message (return.first == false if deq() was disabled)
  std::pair<bool,T>deq(){
    // NOTE! Not yet done
  }
  // cancel deq operations (will also release blocking threads)
  void disable_deq(bool disable){
    ipc::scoped_lock<ipc::named_mutex>lock(ipcmtx_);
    deq_enabled_=!disable;
    ipccond_.notify_all();
  }
  // cancel enq operations (will also release blocking threads)
  void disable_enq(bool disable){
    ipc::scoped_lock<ipc::named_mutex>lock(ipcmtx_);
    enq_enabled_=!disable;
    ipccond_.notify_all();
  }
  // set max size of queue
  void set_maxsize(std::size_t maxsize){
    ipc::scoped_lock<ipc::named_mutex>lock(ipcmtx_);
    ipccond_.notify_all();
    return maxsize_;
  }
  // check if queue is empty
  bool empty()const{
    // NOTE! Not yet done
  }
  // check if queue is full
  bool full()const{
    // NOTE! Not yet done
  }
  // get #of items in queue
  std::size_t size()const{
    // NOTE! Not yet done
  }
  // get max items in queue
  std::size_t maxsize()const{
    ipc::scoped_lock<ipc::named_mutex>lock(ipcmtx_);
    return maxsize_;
  }
  // remove lock variables for queue
  static void removeLockVariables(fs::path const&dir){
    detail::dirqueue_support::removeLockVariables(dir);
  }
private:
  // user specified characteristics of queue
  std::size_t maxsize_;
  fs::path dir_;
  bool removelocks_;

  // serialization/deserialization functions
  DESER deser_;
  SERIAL serial_;

  // state of queue
  bool deq_enabled_=true;
  bool enq_enabled_=true;

  // mutex/condition variables
  mutable boost::interprocess::named_mutex ipcmtx_;
  mutable boost::interprocess::named_condition ipccond_;

  // cache of message
  mutable std::list<fs::path>cache_;
};
}
}
#endif

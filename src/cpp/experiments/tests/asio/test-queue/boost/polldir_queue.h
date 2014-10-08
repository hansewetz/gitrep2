/*
NOTE!	
	we should remove max size in polldir_queue and have wait_enq() always return true
	for this type of queue it's too cimplicated otherwise
*/
#ifndef __POLLDIR_QUEUE_H__
#define __POLLDIR_QUEUE_H__
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

// a simple threadsafe/interprocess-safe queue using directory as queue and files as storage media for queue items
// (mutex/condition variable names are derived from the queue name)
// (enq/deq have locks around them so that we cannot read partial messages)
template<typename T,typename DESER,typename SERIAL>
class polldir_queue{
public:
  // typedef for value stored in queue
  // (need this so we can create an item with default ctor)
  using value_type=T;

  // typedef for serialiser/de-serialiser
  using deserialiser=DESER;
  using serialiser=SERIAL;

  // ctors,assign,dtor
  // (if maxsize == 0 checking for max numbert of queue elements is ignored)
  polldir_queue(std::size_t maxsize,size_t pollms,fs::path const&dir,DESER deser,SERIAL serial,bool removelocks):
      maxsize_(maxsize),pollms_(pollms),dir_(dir),deser_(deser),serial_(serial),removelocks_(removelocks),
      ipcmtx_(ipc::open_or_create,detail::dirqueue_support::getMutexName(dir).c_str()),ipccond_(ipc::open_or_create,detail::dirqueue_support::getCondName(dir).c_str()){
    // make sure path is a directory
    if(!fs::is_directory(dir_))throw std::logic_error(std::string("polldir_queue::polldir_queue: dir_: ")+dir.string()+" is not a directory");
  }
  polldir_queue(polldir_queue const&)=delete;
  polldir_queue(polldir_queue&&)=default;
  polldir_queue&operator=(polldir_queue const&)=delete;
  polldir_queue&operator=(polldir_queue&&)=default;
  ~polldir_queue(){
    if(removelocks_)removeLockVariables(dir_);
  }
  // put a message into queue
  // (returns true if message was enqueued, false if enqueing was disabled)
  bool enq(T t){
    // loop until there is room in the queue or until enquing is disabled
    ipc::scoped_lock<ipc::named_mutex>lock(ipcmtx_);
    while(true){
      // sleep with poll intervall or until someone alerts us
      pt::ptime const abstm{pt::microsec_clock::local_time()+pt::milliseconds(pollms_)};
      bool tmo=!ipccond_.timed_wait(lock,abstm,[&](){return !enq_enabled_||!fullNolock();});

      // if enq is disabled we'll return
      if(!enq_enabled_)return false;

      // if we timed out must check if queue is full
      if(tmo&&fullNolock())continue;

      // we know we can now write message
      detail::dirqueue_support::write(t,dir_,serial_);
      ipccond_.notify_all();
      return true;
    }
  }
  // wait until we can put a message in queue
  // (returns false if enqueing was disabled, else true)
  bool wait_enq(){
    // loop until there is room in the queue or until enquing is disabled
    ipc::scoped_lock<ipc::named_mutex>lock(ipcmtx_);
    while(true){
      // sleep with poll intervall or until someone alerts us
      pt::ptime const abstm{pt::microsec_clock::local_time()+pt::milliseconds(pollms_)};
      bool tmo=!ipccond_.timed_wait(lock,abstm,[&](){return !enq_enabled_||!fullNolock();});

      // if enq is disabled we'll return
      if(!enq_enabled_)return false;

      // if we timed out must check if queue is full
      if(tmo&&fullNolock())continue;

      // we know we can now write message
      ipccond_.notify_all();
      return true;
    }
  }
  // dequeue a message (return.first == false if deq() was disabled)
  std::pair<bool,T>deq(){
    // loop until we have a message or until dequeing is disabled
    ipc::scoped_lock<ipc::named_mutex>lock(ipcmtx_);
    while(deq_enabled_){
      // get oldest file
      std::pair<bool,fs::path>oldestFile{nextFile()};
      if(oldestFile.first){
        // convert file into message, remove file and return message
        T ret{detail::dirqueue_support::read<T>(oldestFile.second,deser_)};
        ipccond_.notify_all();
        return std::make_pair(true,ret);
      }
      // sleep with poll intervall or until someone alerts us
      boost::system_time const abstm{boost::get_system_time()+pt::milliseconds(pollms_)};
      ipccond_.timed_wait(lock,abstm,[&](){return !deq_enabled_;});
    }
    // return null ptr if deq is disabled
    return std::make_pair(false,T{});
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
    return maxsize_;
  }
  // check if queue is empty
  bool empty()const{
    ipc::scoped_lock<ipc::named_mutex>lock(ipcmtx_);
    return emptyNolock();
  }
  // check if queue is full
  bool full()const{
    ipc::scoped_lock<ipc::named_mutex>lock(ipcmtx_);
    return fullNolock();
  }
  // get #of items in queue
  std::size_t size()const{
    ipc::scoped_lock<ipc::named_mutex>lock(ipcmtx_);
    return sizeNolock();
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
  // check if queue is full
  // (lock must be held when calling this function)
  bool fullNolock()const{
    // if maxsize_ == 0 we ignore checking the size of the queue
    if(maxsize_==0)return false;
    return sizeNolock()>=maxsize_;
  }
  // check if queue is empty 
  // (lock must be held when calling this function)
  bool emptyNolock()const{
    std::pair<bool,fs::path>oldestFile{nextFile()};
    if(!oldestFile.first)return true;
    return false;
  }
  // get size of queue
  // (lock must be held when calling this function)
  size_t sizeNolock()const{
    return allFiles().size();
  }
  // get oldest file + manage cache_ if needed
  // (lock must be held when calling this function)
  std::pair<bool,fs::path>nextFile()const{
    // only fill cache if cache is empty
    cleanCache();
    fillCache(false);
    if(cache_.empty())return std::make_pair(false,fs::path());
    std::pair<bool,fs::path>ret{std::make_pair(true,cache_.front())};
    cache_.pop_front();
    return ret;
  }
  // get all pending messages and fill cache at the same time
  // (lock must be held when calling this function)
  std::list<fs::path>allFiles()const{
    fillCache(true);
    return cache_;
  }
  // fill cache
  // (must be called when having the lock)
  void fillCache(bool refill)const{
    // trash cache and re-read it if 'refill' is set or if cache is empty
    if(refill||cache_.empty()){
      std::list<fs::path>sortedFiles{detail::dirqueue_support::getTsOrderedFiles(dir_)};
      cache_.swap(sortedFiles);
    }
  }
  // remove any file in cache which do not exist in dir_
  // (must be called when having the lock)
  void cleanCache()const{
    std::list<fs::path>tmpCache;
    for(auto const&p:cache_)
      if(fs::exists(p))tmpCache.push_back(p);
    swap(tmpCache,cache_);
  }
  // user specified characteristics of queue
  std::size_t maxsize_;
  std::size_t pollms_;
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

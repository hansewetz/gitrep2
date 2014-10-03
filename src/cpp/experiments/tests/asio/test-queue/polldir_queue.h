/*
NOTE!	Maybe we should remove the max size of the queue ...?
	We should change polling to inotify - sort out how contention would be handled
*/
#ifndef __POLLDIR_QUEUE_H__
#define __POLLDIR_QUEUE_H__
#include <string>
#include <utility>
#include <list>
#include <map>
#include <fstream>
#include <mutex>
#include <chrono>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace boost{
namespace asio{

namespace fs=boost::filesystem;
namespace ipc=boost::interprocess;
namespace pt= boost::posix_time;

// a simple threadsafe/interprocess-safe queue using directory as queue and files as storage media for queue items
// (mutex/condition variable names are derived from the queue name)
// (enq/deq have locks around them so that we cannot read a partial message)
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
      ipcmtx_(ipc::open_or_create,getMutexName(dir).c_str()),ipccond_(ipc::open_or_create,getCondName(dir).c_str()){
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
      write(t);
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
        T ret{read(oldestFile.second)};
        ipccond_.notify_all();
        return std::make_pair(true,ret);
      }
      // sleep with poll intervall or until someone alerts us
      boost::system_time const abstm{boost::get_system_time()+pt::milliseconds(1000)};
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
    boost::interprocess::named_mutex::remove(getMutexName(dir).c_str());
    boost::interprocess::named_condition::remove(getCondName(dir).c_str());
  }
private:
  // helper write function (lock must be held when calling this function)
  void write(T const&t)const{
    // create a unique filename, open file for writing and serialise object to file (user defined function)
    // (serialization function is a user supplied function - see ctor)
    std::string const id{boost::lexical_cast<std::string>(boost::uuids::random_generator()())};
    fs::path fullpath{dir_/id};
    std::ofstream os{fullpath.string(),std::ofstream::binary};
    if(!os)throw std::runtime_error(std::string("polldir_queue::write: could not open file: ")+fullpath.string());
    serial_(os,t);
    os.close();
  }
  // helper read function (lock must be held when calling this function)
  T read(fs::path const&fullpath)const{
    // open input stream, deserialize stream into an object and remove file
    // (deserialization function is a user supplied function - see ctor)
    std::ifstream is{fullpath.string(),std::ifstream::binary};
    if(!is)throw std::runtime_error(std::string("polldir_queue::read: could not open file: ")+fullpath.string());
    T ret{deser_(is)};
    is.close();
    std::remove(fullpath.string().c_str());
    return ret;
  }
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
      std::multimap<time_t,fs::path>sortedFiles{getTsOrderedFiles(dir_)};
      cache_.clear();
      for(auto const&f:sortedFiles)cache_.push_back(f.second);
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
  // get all filenames in time sorted order in a dircetory
  static std::multimap<time_t,fs::path>getTsOrderedFiles(fs::path const&dir){
    // need a map with key=time, value=filename
    typedef std::multimap<time_t,fs::path>time_file_map_t;
    time_file_map_t time_file_map;

    // insert all files together with time as key into map
    fs::directory_iterator dir_end_iter;
    for(fs::directory_iterator it(dir);it!=dir_end_iter;++it){
      if(!is_regular_file(*it))continue;
      time_t time_stamp(last_write_time(*it));
      time_file_map.insert(time_file_map_t::value_type(time_stamp,*it));
    }
    return time_file_map;
  }
  // create a name for mutex for this dircetory queue queue
  static std::string getMutexName(fs::path const&dir){
    std::string sdir{dir.string()};
    std::replace_if(sdir.begin(),sdir.end(),[](char c){return c=='/'||c=='.';},'_');
    return sdir;
  }
  // create a name for condition variable for this directory queue
  static std::string getCondName(fs::path const&dir){
    std::string sdir{dir.string()};
    std::replace_if(sdir.begin(),sdir.end(),[](char c){return c=='/'||c=='.';},'_');
    return sdir;
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

/*
NOTE!	Maybe we should remove the max size of the queue ...?
	We should change polling to inotify - sort out how contention would be handled
*/
#ifndef __POLLDIR_QUEUE_H__
#define __POLLDIR_QUEUE_H__
#include <utility>
#include <list>
#include <map>
#include <fstream>
#include <mutex>
#include <boost/filesystem.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>

namespace boost{
namespace asio{

namespace fs=boost::filesystem;
namespace ipc=boost::interprocess;
namespace pt= boost::posix_time;

// a simple threadsafe queue using directories as queue and files for storing messages
// (mutex/condition variable names are derived from the queue name)
// (enq/deq have locks around them so that we cannot read 'half' a message)
template<typename T,typename FR>
class polldir_queue{
public:
  // typedef for value stored in queue
  // (need this so we can create an item with default ctor)
  using value_type=T;

  // ctors,assign,dtor
  polldir_queue(std::size_t maxsize,size_t pollms,fs::path const&dir,FR fr,bool removelocks):
      maxsize_(maxsize),pollms_(pollms),dir_(dir),fr_(fr),removelocks_(removelocks),
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
  bool enq(T t){
    std::unique_lock<std::mutex>lock(ipcmtx_);
    ipccond_.wait(lock,[&](){return !enq_enabled_||sizeNolock()<maxsize_;});
    if(!enq_enabled_)return false;
    write(t);
    ipccond_.notify_all();
    return true;
  }
  // wait until we can put a message in queue
  bool wait_enq(){
    std::unique_lock<std::mutex>lock(ipcmtx_);
    ipccond_.wait(lock,[&](){return !enq_enabled_||sizeNolock()()<maxsize_;});
    if(!enq_enabled_)return false;
    ipccond_.notify_all();
    return true;
  }
  // dequeue a message (return.first == false if deq() was disabled)
  std::pair<bool,T>deq(){
    std::unique_lock<std::mutex>lock(ipcmtx_);
    ipccond_.wait(lock,[&](){return !deq_enabled_||!emptyNolock();});

    // if deq is disabled or queue is empty return 
    if(!deq_enabled_)return std::make_pair(false,T{});

    // check if we have a message
    std::pair<bool,fs::path>oldestFile{nextFile()};
    if(!oldestFile.first)return std::make_pair(false,T{});

    // we have a message, read it and return
    T ret{read(oldestFile.second)};
    ipccond_.notify_all();
    return ret;
  }
  // cancel deq operations (will also release blocking threads)
  void disable_deq(bool disable){
    ipc::scoped_lock<ipc::named_mutex>lock(ipcmtx_);
    enq_enabled_=!disable;
    ipccond_.notify_all();
  }
  // cancel enq operations (will also release blocking threads)
  void disable_enq(bool disable){
    ipc::scoped_lock<ipc::named_mutex>lock(ipcmtx_);
    deq_enabled_=!disable;
    ipccond_.notify_all();
  }
  // set max size of queue
  void set_maxsize(std::size_t maxsize){
    std::lock_guard<std::mutex>(ipcmtx_);
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
    return sizeNolock().size()>=maxsize_;
  }
  // get #of items in queue
  std::size_t size()const{
    ipc::scoped_lock<ipc::named_mutex>lock(ipcmtx_);
    return sizeNolock().size();
  }
  // get max items in queue
  std::size_t maxsize()const{
    std::lock_guard<std::mutex>(ipcmtx_);
    return maxsize_;
  }
  // remove lock variables for queue
  static void removeLockVariables(fs::path const&dir){
    boost::interprocess::named_mutex::remove(getMutexName(dir).c_str());
    boost::interprocess::named_condition::remove(getCondName(dir).c_str());
  }
private:
  // helper write function (lock must be held when calling this functions)
  void write(T const&t)const{
    // NOTE! Not yet done
    // how should be generate file names - uuid?
  }
  // helper read function (lock must be held when calling this functions)
  T read(fs::path const&filename)const{
    // open input stream and deserialize stream into an object
    fs::path ffile{dir_/filename};
    std::istream is{std::ifstream(ffile.string(),std::ifstream::binary)};
    if(!is)throw std::runtime_error(std::string("polldir_queue::read: could not open file: ")+ffile.string());
    return fr_(is);
  }
  // check for empty without locking
  bool emptyNolock()const{
    std::pair<bool,fs::path>oldestFile{nextFile()};
    if(!oldestFile.first)return true;
    return false;
  }
  // get oldest file + manage cache_ if needed
  // (must be called when having the lock)
  std::pair<bool,fs::path>nextFile()const{
    // only fill cache if cache is empty
    cleanCache();
    fillCache(false);
    if(cache_.empty())return std::make_pair(false,fs::path());
    std::pair<bool,fs::path>ret{std::make_pair(true,cache_.front())};
    cache_.pop_front();
    return ret;
  }
  // get size of queue
  size_t sizeNolock()const{
    return allFiles.size();
  }
  // (must be called when having the lock)
  // get all pending messages and fill cache at the same time
  // (must be called when having the lock)
  std::list<fs::path>allFiles()const{
    cleanCache();
    fillCache(true);
    return cache_;
  }
  // fill cache
  // (must be called when having the lock)
  void fillCache(bool refill)const{
    // trash cache and re-read it if 'refill' is set or if cache is empty
    if(refill||cache_.empty()){
      std::multimap<time_t,fs::path>sortedFiles{getTsOrderedFiles()};
      cache_.clear();
      for(auto const&f:sortedFiles)cache_.push_back(f.second);
    }
  }
  // fill cache of files
  // (must be called when having the lock)
  void cleanCache()const{
    std::list<fs::path>tmpCache;
    for(auto const&p:cache_){
      if(fs::exists(p))tmpCache.push_back(p);
    }
    swap(tmpCache,cache_);
  }
  // create a name for mutex for this dircetory queue queue
  static std::string getMutexName(fs::path const&dir){
    std::string sdir{dir.string()};
    std::replace_if(sdir.begin(),sdir.end(),[](char c){return c=='/'||c=='.';},'_');
    return sdir;
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
  FR fr_;

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

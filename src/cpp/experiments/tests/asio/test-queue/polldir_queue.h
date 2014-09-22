#ifndef __POLLDIR_QUEUE_H__
#define __POLLDIR_QUEUE_H__
#include <memory>
#include <list>
#include <utility>
#include <boost/filesystem.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>

namespace fs=boost::filesystem;
namespace ipc=boost::interprocess;
namespace pt= boost::posix_time;

namespace boost{
namespace asio{

// a simple threadsafe queue using directories as queue and files for storing messages
// (mutex/condition variable names are derived from the queue name)
// (enq/deq have locks around them so that we cannot read 'half' a message)
template<typename T>
class polldir_queue{
public:
  // typedef for value stored in queue
  // (need this so we can create an item with default ctor)
  using value_type=T;

  // ctors,assign,dtor
  polldir_queue(std::size_t maxsize,size_t pollms,fs::path const&dir,bool removelocks):
    maxsize_(maxsize),pollms_(pollms),dir_(dir),removelocks_(removelocks),
    mtx_(ipc::open_or_create,getMutexName(dir).c_str()),cond_(ipc::open_or_create,getCondName(dir).c_str()){
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
    // NOTE! Not yet done
  }
  // wait until we can put a message in queue
  bool wait_enq(){
    // NOTE! Not yet done
    return true;
  }
  // dequeue a message (return.first == false if deq() was disabled)
  std::pair<bool,T>deq(){
    // NOTE! Not yet done
  }
  // cancel deq operations (will also release blocking threads)
  void disable_deq(bool disable){
    // NOTE! Not yet done
  }
  // cancel enq operations (will also release blocking threads)
  void disable_enq(bool disable){
    // NOTE! Not yet done
  }
  // set max size of queue
  void set_maxsize(std::size_t maxsize){
    // NOTE! Not yet done
  }
  // check if queue is empty
  bool empty()const{
    ipc::scoped_lock<ipc::named_mutex>lock(mtx_);
    return emptyNolock();
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
    // NOTE! Not yet done
  }
  // remove lock variables for queue
  static void removeLockVariables(boost::filesystem::path const&dir){
    boost::interprocess::named_mutex::remove(getMutexName(dir).c_str());
    boost::interprocess::named_condition::remove(getCondName(dir).c_str());
  }
private:
  // helper functions (lock must be held when calling these functions)
  void write(std::shared_ptr<T>msg)const;
  std::shared_ptr<T>read(boost::filesystem::path const&filename)const;
  bool emptyNolock()const;
  std::pair<bool,boost::filesystem::path>nextFile()const;
  std::list<boost::filesystem::path>allFiles()const;
  void fillCache(bool)const;
  void cleanCache()const;

  // create a name for mutex for this dircetory queue queue
  static std::string getMutexName(boost::filesystem::path const&dir){
    std::string sdir{dir.string()};
    std::replace_if(sdir.begin(),sdir.end(),[](char c){return c=='/'||c=='.';},'_');
    return sdir;
  }
  // create a name for condition variable for this directory queue
  static std::string getCondName(boost::filesystem::path const&dir){
    std::string sdir{dir.string()};
    std::replace_if(sdir.begin(),sdir.end(),[](char c){return c=='/'||c=='.';},'_');
    return sdir;
  }
  // user specified characteristics of queue
  std::size_t maxsize_;
  std::size_t pollms_;
  boost::filesystem::path dir_;
  bool removelocks_;

  // state of queue
  bool deq_enabled_=true;
  bool enq_enabled_=true;

  // internally controlled state
  mutable boost::interprocess::named_mutex mtx_;
  mutable boost::interprocess::named_condition cond_;
  mutable std::list<boost::filesystem::path>cache_;
};
}
}
#endif

#ifndef __SIMPLEQ_H__
#define __SIMPLEQ_H__
#include <utility>
#include <queue>
#include <mutex>
#include <condition_variable>

// a simple thread safe queue
template<typename T,typename Container=std::queue<T>>
class simpleq{
public:
  // ctors,assign,dtor
  simpleq()=default;
  simpleq(simpleq const&)=delete;
  simpleq(simpleq&&)=default;
  simpleq&operator=(simpleq const&)=delete;
  simpleq&operator=(simpleq&&)=default;
  ~simpleq()=default;

  // put a message into queue
  bool enq(T t){
    std::lock_guard<std::mutex>lock(mtx_);
    q_.push(t);
    cond_.notify_all();
  }
  // dequeue a message (return.first == false if deq() was cancelled)
  std::pair<bool,T>deq(){
    std::unique_lock<std::mutex>lock(mtx_);
    cond_.wait(lock,[&](){return !deq_enabled_||!q_.empty();});
  
    // if deq iscancelled or queue is empty return 
    if(!deq_enabled_||q_.empty()){
      return std::make_pair(false,T{});
    }
    // check if we have a message
    std::pair<bool,T>ret{std::make_pair(true,q_.front())};
    q_.pop();
    return ret;
  }
  // cancel deq operations
  void disable_deq(bool disable){
    std::unique_lock<std::mutex>lock(mtx_);
    deq_enabled_=!disable;
    cond_.notify_all();
  }
  // check if queue is empty
  bool empty()const{
    std::unique_lock<std::mutex>lock(mtx_);
    return q_.empty();
  }
private:
  mutable std::mutex mtx_;
  mutable std::condition_variable cond_;
  bool deq_enabled_=true;
  Container q_;
};
#endif

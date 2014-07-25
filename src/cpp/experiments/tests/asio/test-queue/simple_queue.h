#ifndef __SIMPLE_QUEUE_H__
#define __SIMPLE_QUEUE_H__
#include <utility>
#include <queue>
#include <mutex>
#include <condition_variable>
namespace boost{
namespace asio{

// a simple thread safe queue
template<typename T,typename Container=std::queue<T>>
class simple_queue{
public:
  // ctors,assign,dtor
  simple_queue()=default;
  simple_queue(simple_queue const&)=delete;
  simple_queue(simple_queue&&)=default;
  simple_queue&operator=(simple_queue const&)=delete;
  simple_queue&operator=(simple_queue&&)=default;
  ~simple_queue()=default;

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
}
}
#endif

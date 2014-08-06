#ifndef __SIMPLE_QUEUE_H__
#define __SIMPLE_QUEUE_H__
#include <utility>
#include <queue>
#include <mutex>
#include <condition_variable>
namespace boost{
namespace asio{

// a simple thread safe queue used as default queue in boost::asio::queue_listener
template<typename T,typename Container=std::queue<T>>
class simple_queue{
public:
  // typedef for value stored in queue
  // (need this so we can create an item with default ctor)
  using value_type=T;

  // ctors,assign,dtor
  simple_queue(std::size_t maxsize):maxsize_(maxsize){}
  simple_queue(simple_queue const&)=delete;
  simple_queue(simple_queue&&)=default;
  simple_queue&operator=(simple_queue const&)=delete;
  simple_queue&operator=(simple_queue&&)=default;
  ~simple_queue()=default;

  // put a message into queue
  bool enq(T t){
    std::unique_lock<std::mutex>lock(mtx_);
    cond_.wait(lock,[&](){return !enq_enabled_||q_.size()<maxsize_;});
    if(!enq_enabled_)return false;
    q_.push(t);
    cond_.notify_all();
    return true;
  }
  // dequeue a message (return.first == false if deq() was disabled)
  std::pair<bool,T>deq(){
    std::unique_lock<std::mutex>lock(mtx_);
    cond_.wait(lock,[&](){return !deq_enabled_||!q_.empty();});
  
    // if deq is disabled or queue is empty return 
    if(!deq_enabled_||q_.empty()){
      return std::make_pair(false,T{});
    }
    // check if we have a message
    std::pair<bool,T>ret{std::make_pair(true,q_.front())};
    q_.pop();
    cond_.notify_all();
    return ret;
  }
  // cancel deq operations (will also release blocking threads)
  void disable_deq(bool disable){
    std::unique_lock<std::mutex>lock(mtx_);
    deq_enabled_=!disable;
    cond_.notify_all();
  }
  // cancel enq operations (will also release blocking threads)
  void disable_enq(bool disable){
    std::unique_lock<std::mutex>lock(mtx_);
    enq_enabled_=!disable;
    cond_.notify_all();
  }
  // set max size of queue
  void set_maxsize(std::size_t maxsize){
    std::unique_lock<std::mutex>lock(mtx_);
    maxsize_=maxsize;
    cond_.notify_all();
  }
  // check if queue is empty
  bool empty()const{
    std::unique_lock<std::mutex>lock(mtx_);
    return q_.empty();
  }
  // get #of items in queue
  std::size_t size()const{
    std::unique_lock<std::mutex>lock;
    return q_.size();
  }
  // get max items in queue
  std::size_t maxsize()const{
    std::unique_lock<std::mutex>lock(mtx_);
    return maxsize_;
  }
private:
  std::size_t maxsize_;
  mutable std::mutex mtx_;
  mutable std::condition_variable cond_;
  bool deq_enabled_=true;
  bool enq_enabled_=true;
  Container q_;
};
}
}
#endif

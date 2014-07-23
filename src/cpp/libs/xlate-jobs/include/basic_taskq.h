/* NOTE!
Notes:
	- We have to reet the queue after each completed async operation
	  we should probably have the Impl object maintain the TaskQueue so we don't need to pass it as a parameter each time
*/
#ifndef __basic_taskq_H__
#define __basic_taskq_H__
#include "xlate-jobs/TaskQueue.h"
#include "xlate-jobs/TranslationTask.h"

#include <boost/asio.hpp> 
#include <boost/system/error_code.hpp> 

#include <cstddef> 
#include <memory> 
#include <thread> 
namespace xlate{

// forward decl
class taskq_impl;
template<typename Impl=taskq_impl>class basic_taskq_service;

// --- IO Object (used by client) -----------------------------
template<typename Service>
class basic_taskq:public boost::asio::basic_io_object<Service>{
public: 
  // ctor (note: 'consruct()' is called in base class)
  explicit basic_taskq(boost::asio::io_service&io_service,std::shared_ptr<TaskQueue>q):
      boost::asio::basic_io_object<Service>(io_service) {
    // set queue for in implementation - the queue will never change
    this->implementation->setQueue(q);

// NOTE!
std::cerr<<"IN CTOR"<<std::endl;
  } 
  // sync deq operation
  std::shared_ptr<TranslationTask>deq(){
    return this->service.deq(this->implementation); 
  } 
  // async sync deq operation
  template <typename Handler> 
  void async_deq(Handler handler) {
    this->service.async_deq(this->implementation,handler); 
  } 
}; 
// typedef for using standard service object
using TaskQueueIOService=basic_taskq<basic_taskq_service<>>;

// --- service class for basic_taskq -----------------------------
// (for one io_service, only one object created)
template<typename Impl>
class basic_taskq_service:public boost::asio::io_service::service{
public:
 static boost::asio::io_service::id id; 

  // ctor
  explicit basic_taskq_service(boost::asio::io_service&io_service) :
      boost::asio::io_service::service(io_service), 
      async_work_(new boost::asio::io_service::work(async_io_service_)), 
      async_thread_([&](){async_io_service_.run();}){
  } 
  // dtor (empty work queue and join async service thread)
  ~basic_taskq_service(){
// NOTE!
std::cerr<<"(1) DTOR"<<std::endl;
    async_work_.reset(); 
std::cerr<<"(2) DTOR"<<std::endl;
    async_io_service_.stop(); 
std::cerr<<"(3) DTOR"<<std::endl;
    async_thread_.join(); 
std::cerr<<"(4) DTOR"<<std::endl;
  }
  // get a typedef  for the implementation
  typedef std::shared_ptr<Impl>implementation_type; 

  // mandatory (construct an implementation object)
  void construct(implementation_type&impl){ 
      impl.reset(new Impl(std::shared_ptr<TaskQueue>(nullptr))); 
// NOTE!
std::cerr<<"IN construct()"<<std::endl;
  } 
  // mandatory (destroy an implementation object)
  void destroy(implementation_type&impl){
// NOTE!
std::cerr<<"(1) IMPL DESTROY"<<std::endl;
    impl->destroy(); 
std::cerr<<"(2) IMPL DESTROY"<<std::endl;
    impl.reset(); 
std::cerr<<"(3) IMPL DESTROY"<<std::endl;
  }
  // sync deq operation
  std::shared_ptr<TranslationTask>deq(implementation_type&impl){
    boost::system::error_code ec; 
    std::pair<bool,std::shared_ptr<TranslationTask>>ret{impl->deq(ec)};
    boost::asio::detail::throw_error(ec); 
    return ret.second;
  } 
  // function object calling implementation object
  template <typename Handler> 
  class deq_operation{ 
  public: 
    // ctor
    deq_operation(implementation_type&impl,boost::asio::io_service &io_service,Handler handler):
        impl_(impl),io_service_(io_service),work_(io_service),handler_(handler) {
    } 
    // function calling implementation object in separate thread
    void operator()(){ 
      // (must check if implementation object still exist - use of weak_ptr)
      implementation_type impl=impl_.lock(); 
      if(impl){
        // go ahead and do blocking deq() call on implementation object
        // (only post if we did not get a null message)
        boost::system::error_code ec; 
        std::pair<bool,std::shared_ptr<TranslationTask>>ret{impl->deq(ec)};
        if(ret.first)this->io_service_.post(boost::asio::detail::bind_handler(handler_,ec,ret.second));
      } else{
        // implementation object no longer exist
        std::shared_ptr<TranslationTask>task=std::shared_ptr<TranslationTask>(nullptr);
        this->io_service_.post(boost::asio::detail::bind_handler(handler_,boost::asio::error::operation_aborted,task));
      } 
    } 
  private: 
    std::weak_ptr<Impl>impl_; 
    boost::asio::io_service&io_service_; 
    boost::asio::io_service::work work_; 
    Handler handler_; 
  }; 
  // async sync deq operation
  template <typename Handler> 
  void async_deq(implementation_type&impl,Handler handler){
    // call indirectly by posting a function object so blocking call to deq() will run in a separate thread
    this->async_io_service_.post(deq_operation<Handler>(impl,this->get_io_service(),handler)); 
  } 
private:
  // shutdown service
  void shutdown_service(){
    // nothing to do here
  } 
  // private data
  boost::asio::io_service async_io_service_; 
  boost::scoped_ptr<boost::asio::io_service::work>async_work_; 
  std::thread async_thread_; 
};
// definition of id of service
template <typename Impl> 
boost::asio::io_service::id basic_taskq_service<Impl>::id; 

// --- implementation of taskq -----------------------------

// NOTE! implement so that implementation runs its own asio io_service

class taskq_impl{
public:
  // ctor
  taskq_impl(std::shared_ptr<TaskQueue>tq):tq_(tq){}

  // dtor
  ~taskq_impl(){
    // nothing to do here, we are only dealing with consumer side
  }
  // destroy implementation of service
  void destroy(){
    // nothing to do here, we are only dealing with consumer side
  } 
  // deque message
  // (if we got a nullptr from deq(), we set 'return.first = false', else true)
  std::pair<bool,std::shared_ptr<TranslationTask>>deq(boost::system::error_code&ec){ 
    std::shared_ptr<TranslationTask>task=tq_->deq(true);
    ec = boost::system::error_code();
// NOTE!
std::cerr<<"BEFORE DEQ()"<<std::endl;
    std::pair<bool,std::shared_ptr<TranslationTask>>ret{make_pair(true,task)};
std::cerr<<"AFTER DEQ()"<<std::endl;
    if(task.get()==nullptr){
      ret.first=false;
      ec=boost::asio::error::operation_aborted;
    }
    return ret;
  } 
// NOTE! Should have this one as private and be a friend of the class calling it
  // set queue to listen on
  void setQueue(std::shared_ptr<TaskQueue>tq){
    tq_=tq;
  }
private:
  std::shared_ptr<TaskQueue>tq_;
};
}
#endif

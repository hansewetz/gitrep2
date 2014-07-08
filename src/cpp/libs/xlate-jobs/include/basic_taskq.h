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
#include <boost/shared_ptr.hpp> 
#include <boost/system/error_code.hpp> 
#include <boost/thread.hpp> 
#include <boost/bind.hpp> 

#include <cstddef> 
#include <memory> 
namespace xlate{

// forward decl
class taskq_impl;
template<typename Impl=taskq_impl>class basic_taskq_service;

// --- IO Object (used by client) -----------------------------
template<typename Service>
class basic_taskq:public boost::asio::basic_io_object<Service>{
public: 
  // ctor
  explicit basic_taskq(boost::asio::io_service&io_service):
      boost::asio::basic_io_object<Service>(io_service) {
  } 
  // sync deq operation
  std::shared_ptr<TranslationTask>deq(std::shared_ptr<TaskQueue>tq){
    return this->service.deq(this->implementation,tq); 
  } 
  // async sync deq operation
  template <typename Handler> 
  void async_deq(std::shared_ptr<TaskQueue>tq,Handler handler) {
    this->service.async_deq(this->implementation,tq,handler); 
  } 
}; 
// typedef for using standard service object
using TaskQueueIOService=basic_taskq<basic_taskq_service<>>;

// --- service class for basic_taskq -----------------------------
template<typename Impl>
class basic_taskq_service:public boost::asio::io_service::service{
public:
 static boost::asio::io_service::id id; 

  // ctor
  explicit basic_taskq_service(boost::asio::io_service&io_service) :
      boost::asio::io_service::service(io_service), 
      async_work_(new boost::asio::io_service::work(async_io_service_)), 
      async_thread_(boost::bind(&boost::asio::io_service::run,&async_io_service_)){
  } 
  // dtor
  ~basic_taskq_service(){
    async_work_.reset(); 
    async_io_service_.stop(); 
    async_thread_.join(); 
  }
  // get a typedef  for the implementation
  typedef boost::shared_ptr<Impl>implementation_type; 

  // mandatory
  void construct(implementation_type&impl){ 
      impl.reset(new Impl()); 
  } 
  // mandatory
  void destroy(implementation_type&impl){
    impl->destroy(); 
    impl.reset(); 
  }
  // sync deq operation
  std::shared_ptr<TranslationTask>deq(implementation_type&impl,std::shared_ptr<TaskQueue>tq){
    boost::system::error_code ec; 
    std::shared_ptr<TranslationTask>ret=impl->deq(tq,ec); 
    boost::asio::detail::throw_error(ec); 
    return ret;
  } 
  // async deq() operation in its own thread
  template <typename Handler> 
  class deq_operation{ 
  public: 
    // ctor
    deq_operation(implementation_type&impl,boost::asio::io_service &io_service,std::shared_ptr<TaskQueue>tq, Handler handler):
        impl_(impl),io_service_(io_service),work_(io_service),tq_(tq),handler_(handler) {
  } 
  // function called by thread
  void operator()(){ 
    implementation_type impl=impl_.lock(); 
    if(impl){
      boost::system::error_code ec; 
      std::shared_ptr<TranslationTask>task=impl->deq(tq_,ec); 
      this->io_service_.post(boost::asio::detail::bind_handler(handler_,ec,task));
    } else{
      std::shared_ptr<TranslationTask>task=std::shared_ptr<TranslationTask>(nullptr);
      this->io_service_.post(boost::asio::detail::bind_handler(handler_,boost::asio::error::operation_aborted,task));
    } 
  } 
  private: 
    boost::weak_ptr<Impl> impl_; 
    boost::asio::io_service &io_service_; 
    boost::asio::io_service::work work_; 
    std::shared_ptr<TaskQueue>tq_;
    Handler handler_; 
  }; 
  // async sync deq operation
  template <typename Handler> 
  void async_deq(implementation_type&impl,std::shared_ptr<TaskQueue>tq,Handler handler){
    this->async_io_service_.post(deq_operation<Handler>(impl,this->get_io_service(),tq,handler)); 
  } 
private:
  // shutdown service
  void shutdown_service(){
    // must be present - nothing to do here
  } 
  // private data
  boost::asio::io_service async_io_service_; 
  boost::scoped_ptr<boost::asio::io_service::work>async_work_; 
  boost::thread async_thread_; 
};
// definition of id of service
template <typename Impl> 
boost::asio::io_service::id basic_taskq_service<Impl>::id; 

// --- implementation of taskq -----------------------------
class taskq_impl{
public:
  // ctor
  taskq_impl(){}

  // dtor
  ~taskq_impl(){
    // NOTE! stop queue here (need stop method on queue)
  }
  // destroy implementation of service
  void destroy(){
    // NOTE! cancel an blocking deq() on queue here (need stop method here)
  } 
  // deque message
  std::shared_ptr<TranslationTask>deq(std::shared_ptr<TaskQueue>tq,boost::system::error_code&ec){ 
    std::shared_ptr<TranslationTask>task=tq->deq(true);
    ec = boost::system::error_code(); 
    return task;
  } 
};
}
#endif

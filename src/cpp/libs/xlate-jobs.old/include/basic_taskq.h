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
  // ctor
  explicit basic_taskq(boost::asio::io_service&io_service):
      boost::asio::basic_io_object<Service>(io_service) {
  } 
  // async deq operation
  template <typename Handler> 
  void async_deq(std::shared_ptr<TaskQueue>tq,Handler handler) {
    this->service.async_deq(this->implementation,tq,handler); 
  } 
}; 
// typedef for using standard service object
using TaskQueueIOService=basic_taskq<basic_taskq_service<>>;

// --- service class for basic_taskq -----------------------------
// (for one io_service, only one object created)
template<typename Impl>
class basic_taskq_service:public boost::asio::io_service::service{
public:
  // required to have id of service
 static boost::asio::io_service::id id; 

  // ctor
  explicit basic_taskq_service(boost::asio::io_service&io_service):
      boost::asio::io_service::service(io_service){
  } 
  // dtor
  ~basic_taskq_service(){
  }
  // get a typedef  for implementation
  using implementation_type=std::shared_ptr<Impl>;

  // mandatory (construct an implementation object)
  void construct(implementation_type&impl){ 
      impl.reset(new Impl(this->get_io_service())); 
  } 
  // mandatory (destroy an implementation object)
  void destroy(implementation_type&impl){
    impl.reset(); 
  }
  // async sync deq operation
  template <typename Handler> 
  void async_deq(implementation_type&impl,std::shared_ptr<TaskQueue>tq,Handler handler){
    // this is a non-blocking operation so we are OK calling impl object in this thread
    impl->async_deq(tq,handler);
  } 
private:
  // shutdown service (required)
  void shutdown_service(){
  } 
};
// definition of id of service (required)
template <typename Impl> 
boost::asio::io_service::id basic_taskq_service<Impl>::id; 

// --- implementation of taskq -----------------------------
class taskq_impl{
public:
  // ctor (set up work queue for io_service so we don't bail out when executing run())
  taskq_impl(boost::asio::io_service&post_io_service):
      impl_work_(new boost::asio::io_service::work(impl_io_service_)), 
      impl_thread_([&](){impl_io_service_.run();}),
      post_io_service_(post_io_service){
  }
  // dtor (clear work queue, stop io service and join thread)
  ~taskq_impl(){
    impl_work_.reset(); 
    impl_io_service_.stop(); 
    if(impl_thread_.joinable())impl_thread_.join(); 
  }
public:
  // deque message (post request to thread)
  template<typename Handler>
  void async_deq(std::shared_ptr<TaskQueue>tq,Handler handler){
    impl_io_service_.post(deq_operation<Handler>(post_io_service_,tq,handler)); 
  }
private:
  // function object calling blocking deq() on queue
  template <typename Handler> 
  class deq_operation{ 
  public: 
    // ctor
    deq_operation(boost::asio::io_service &io_service,std::shared_ptr<TaskQueue>tq, Handler handler):
        io_service_(io_service),work_(io_service),tq_(tq),handler_(handler) {
    } 
    // function calling implementation object - runs in the thread created in ctor
    void operator()(){ 
      // go ahead and do blocking deq() call
      std::shared_ptr<TranslationTask>task=tq_->deq(true);
      boost::system::error_code ec = boost::system::error_code();
      std::pair<bool,std::shared_ptr<TranslationTask>>ret{make_pair(true,task)};
      if(task){
        this->io_service_.post(boost::asio::detail::bind_handler(handler_,ec,ret.second));
      }else{
        std::shared_ptr<TranslationTask>task=std::shared_ptr<TranslationTask>(nullptr);
        this->io_service_.post(boost::asio::detail::bind_handler(handler_,boost::asio::error::operation_aborted,task));
      }
    } 
  private: 
    boost::asio::io_service&io_service_; 
    boost::asio::io_service::work work_; 
    std::shared_ptr<TaskQueue>tq_;
    Handler handler_; 
  }; 
  // private data
  boost::asio::io_service impl_io_service_; 
  boost::scoped_ptr<boost::asio::io_service::work>impl_work_; 
  std::thread impl_thread_; 
  boost::asio::io_service&post_io_service_; 
};
}
#endif

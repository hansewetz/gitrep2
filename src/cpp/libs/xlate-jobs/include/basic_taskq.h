/* TODO
	- read through the asio code , especially io_service
	- fix TaskQueue by counting number of listeners + updating blockDeq() to stop all listeners + rename blockDeq() --> disableDeq()
	- potentially get rid of the io service in service object and call async_deq() directly on implementation
	- add sync deq also on IO Object
	- try with multiple different queues, should make no difference
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
  explicit basic_taskq(boost::asio::io_service&io_service):
      boost::asio::basic_io_object<Service>(io_service) {
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
std::cerr<<"(0) SERVICE CTOR"<<std::endl<<std::flush;
  } 
  // dtor (empty work queue and join async service thread)
  ~basic_taskq_service(){
std::cerr<<"(1) SERVICE DTOR"<<std::endl<<std::flush;
    async_work_.reset(); 
    async_io_service_.stop(); 
    if(async_thread_.joinable())async_thread_.join(); 
std::cerr<<"(2) SERVICE DTOR"<<std::endl<<std::flush;
  }
  // get a typedef  for the implementation
  typedef std::shared_ptr<Impl>implementation_type; 

  // mandatory (construct an implementation object)
  void construct(implementation_type&impl){ 
      impl.reset(new Impl(this->get_io_service())); 
  } 
  // mandatory (destroy an implementation object)
  void destroy(implementation_type&impl){
std::cerr<<"(1) SERVICE DESTROY"<<std::endl<<std::flush;
    impl->destroy(); 
    impl.reset(); 
std::cerr<<"(2) SERVICE DESTROY"<<std::endl<<std::flush;
  }
  // function object calling implementation object
  template <typename Handler> 
  class deq_operation{ 
  public: 
    // ctor
    deq_operation(implementation_type&impl,boost::asio::io_service &io_service,std::shared_ptr<TaskQueue>tq, Handler handler):
        impl_(impl),io_service_(io_service),work_(io_service),tq_(tq),handler_(handler) {
    } 
    // function calling implementation object in separate thread
    void operator()(){ 
      // (must check if implementation object still exist - use of weak_ptr)
      implementation_type impl=impl_.lock(); 
      if(impl){
        impl->async_deq(tq_,handler_);
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
    std::shared_ptr<TaskQueue>tq_;
    Handler handler_; 
  }; 
  // async sync deq operation
  template <typename Handler> 
  void async_deq(implementation_type&impl,std::shared_ptr<TaskQueue>tq,Handler handler){
    // call indirectly by posting a function object so blocking call to deq() will run in a separate thread
    this->async_io_service_.post(deq_operation<Handler>(impl,this->get_io_service(),tq,handler)); 
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
class taskq_impl{
public:
  // ctor
  taskq_impl(boost::asio::io_service&post_io_service):
      impl_work_(new boost::asio::io_service::work(impl_io_service_)), 
      impl_thread_([&](){impl_io_service_.run();}),
      post_io_service_(post_io_service){
std::cerr<<"(0) IMPL CTOR"<<std::endl<<std::flush;
  }
  // dtor
  ~taskq_impl(){
std::cerr<<"(1) IMPL DTOR"<<std::endl<<std::flush;
    impl_work_.reset(); 
    impl_io_service_.stop(); 
    if(impl_thread_.joinable())impl_thread_.join(); 
std::cerr<<"(2) IMPL DTOR"<<std::endl<<std::flush;
  }
  // destroy implementation of service
  void destroy(){
    // NOTE! Not yet done
std::cerr<<"(1) IMPL DESTROY"<<std::endl<<std::flush;
    if(tq_)tq_->blockDeq();
std::cerr<<"(2) IMPL DESTROY"<<std::endl<<std::flush;
  } 
  // function object calling deq() on queue
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
      if(ret.first){
        this->io_service_.post(boost::asio::detail::bind_handler(handler_,ec,ret.second));
      }else{
std::cerr<<"(0) IMPL GOT NULLPOINTER"<<std::endl<<std::flush;
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
  // deque message
  template<typename Handler>
  void async_deq(std::shared_ptr<TaskQueue>tq,Handler handler){
    // NOTE! Hack to set the tq_ mebre this way
    tq_=tq;
    impl_io_service_.post(deq_operation<Handler>(post_io_service_,tq,handler)); 
  }
private:
  // private data
  boost::asio::io_service impl_io_service_; 
  boost::scoped_ptr<boost::asio::io_service::work>impl_work_; 
  std::thread impl_thread_; 
  boost::asio::io_service&post_io_service_; 
  std::shared_ptr<TaskQueue>tq_;
};
}
#endif

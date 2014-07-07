#ifndef __basic_taskq_H__
#define __basic_taskq_H__
#include "xlate-jobs/TaskQueue.h"

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

// --- IO Object (used by client)
template<typename Service>
class basic_taskq:public boost::asio::basic_io_object<Service>{
public: 
  // ctor
  explicit basic_taskq(boost::asio::io_service&io_service,std::shared_ptr<TaskQueue>,bool block):
      boost::asio::basic_io_object<Service>(io_service) {
  } 
  // sync deq operation
  void deq(){
    return this->service.deq(this->implementation); 
  } 
  // async sync deq operation
  template <typename Handler> 
  void async_deq(Handler handler) {
    this->service.async_deq(this->implementation); 
  } 
}; 
// typedef for using standard service object
using TaskQueueIOService=basic_taskq<basic_taskq_service<>>;

// --- service class for basic_taskq
template<typename Impl>
class basic_taskq_service: public boost::asio::io_service::service {
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
    // NOTE! Not yet done
  }
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
  void deq(){
    // NOTE! Not yet done
  } 
  // async sync deq operation
  template <typename Handler> 
  void async_deq(implementation_type&impl,Handler handler){
    // NOTE! Not yet done
  } 
private:
  // shutdown service
  void shutdown_service(){
    // NOTE! Not yet done
  } 
  // private data
  boost::asio::io_service async_io_service_; 
  boost::scoped_ptr<boost::asio::io_service::work>async_work_; 
  boost::thread async_thread_; 
};
// definition of id of service
template <typename Impl> 
boost::asio::io_service::id basic_taskq_service<Impl>::id; 

// --- implementation of taskq
class taskq_impl{
public:
  // NOTE! Not yet done

  // destroy implementation of service
  void destroy(){
    // NOTE! Not yet done
  } 
};
}
#endif

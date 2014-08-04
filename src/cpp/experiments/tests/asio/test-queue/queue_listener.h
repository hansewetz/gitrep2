#ifndef __QUEUE_LISTENER_H__
#define __QUEUE_LISTENER_H__
#include "simple_queue.h"
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <thread>
namespace boost{
namespace asio{

// forward decl
class queue_listener_impl;
template<typename Impl=queue_listener_impl>class basic_queue_listener_service;

// --- IO Object (used by client) -----------------------------
template<typename Service,typename Queue>
class basic_queue_listener:public boost::asio::basic_io_object<Service>{
public:
  // ctor
  explicit basic_queue_listener(boost::asio::io_service&io_service):
      boost::asio::basic_io_object<Service>(io_service) {
  }
  // async deq operation
  template <typename Handler>
  void async_deq(std::shared_ptr<Queue>q,Handler handler) {
    this->service.async_deq(this->implementation,q,handler);
  }
};
// typedef for using standard service object
template<typename T>
using simple_queue_listener=basic_queue_listener<basic_queue_listener_service<>,simple_queue<T>>;

// --- service class -----------------------------
// (for one io_service, only one object created)
template<typename Impl>
class basic_queue_listener_service:public boost::asio::io_service::service{
public:
  // required to have id of service
 static boost::asio::io_service::id id;

  // ctor
  explicit basic_queue_listener_service(boost::asio::io_service&io_service):
      boost::asio::io_service::service(io_service){
  }
  // dtor
  ~basic_queue_listener_service(){
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
  template <typename Handler,typename Queue>
  void async_deq(implementation_type&impl,std::shared_ptr<Queue>q,Handler handler){
    // this is a non-blocking operation so we are OK calling impl object in this thread
    impl->async_deq(impl,q,handler);
  }
private:
  // shutdown service (required)
  void shutdown_service(){
  }
};
// definition of id of service (required)
template <typename Impl>
boost::asio::io_service::id basic_queue_listener_service<Impl>::id;

// --- implementation -----------------------------
class queue_listener_impl{
public:
  // ctor (set up work queue for io_service so we don't bail out when executing run())
  queue_listener_impl(boost::asio::io_service&post_io_service):
      impl_work_(new boost::asio::io_service::work(impl_io_service_)),
      impl_thread_([&](){impl_io_service_.run();}),
      post_io_service_(post_io_service){
  }
  // dtor (clear work queue, stop io service and join thread)
  ~queue_listener_impl(){
    impl_work_.reset(nullptr);
    impl_io_service_.stop();
    if(impl_thread_.joinable())impl_thread_.join();
  }
public:
  // deque message (post request to thread)
  template<typename Handler,typename Queue>
  void async_deq(std::shared_ptr<queue_listener_impl>impl,std::shared_ptr<Queue>tq,Handler handler){
    impl_io_service_.post(deq_operation<Handler,Queue>(impl,post_io_service_,tq,handler));
  }
private:
  // function object calling blocking deq() on queue
  template <typename Handler,typename Queue>
  class deq_operation{
  public:
    // ctor
    deq_operation(std::shared_ptr<queue_listener_impl>impl,boost::asio::io_service &io_service,std::shared_ptr<Queue>tq, Handler handler):
        wimpl_(impl),io_service_(io_service),work_(io_service),tq_(tq),handler_(handler) {
    }
    // function calling implementation object - runs in the thread created in ctor
    void operator()(){
      // make sure implementation object is still valid
      std::shared_ptr<queue_listener_impl>impl{wimpl_.lock()};

      // if valid, go ahead and do blocking call on queue, otherwise post aborted message
      if(impl){
        std::pair<bool,typename Queue::value_type>ret{tq_->deq()};
        boost::system::error_code ec=(!ret.first?boost::asio::error::operation_aborted:boost::system::error_code());
        this->io_service_.post(boost::asio::detail::bind_handler(handler_,ec,ret.second));
      }else{
        this->io_service_.post(boost::asio::detail::bind_handler(handler_,boost::asio::error::operation_aborted,typename Queue::value_type()));
      }
    }
  private:
    std::weak_ptr<queue_listener_impl>wimpl_;
    boost::asio::io_service&io_service_;
    boost::asio::io_service::work work_;
    std::shared_ptr<Queue>tq_;
    Handler handler_;
  };
  // private data
  boost::asio::io_service impl_io_service_;
  std::unique_ptr<boost::asio::io_service::work>impl_work_;
  std::thread impl_thread_;
  boost::asio::io_service&post_io_service_;
};
}
}
#endif

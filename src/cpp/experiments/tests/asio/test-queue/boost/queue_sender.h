#ifndef __QUEUE_SENDER_H__
#define __QUEUE_SENDER_H__
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <thread>
namespace boost{
namespace asio{

// forward decl
class queue_sender_impl;
template<typename Impl=queue_sender_impl>class basic_queue_sender_service;

// --- IO Object (used by client) -----------------------------
template<typename Service,typename Queue>
class basic_queue_sender:public boost::asio::basic_io_object<Service>{
public:
  // ctor
  explicit basic_queue_sender(boost::asio::io_service&io_service,std::shared_ptr<Queue>q):
      boost::asio::basic_io_object<Service>(io_service),q_(q){
  }
  // async enq operation
  template <typename Handler>
  void async_enq(typename Queue::value_type val,Handler handler) {
    this->service.async_enq(this->implementation,q_,val,handler);
  }
  // sync enq operation (blocking)
  void sync_enq(typename Queue::value_type val){
    this->service.sync_enq(this->implementation,q_,val);
  }
  // wait until we can put a message in queue in async mode
  template <typename Handler>
  void async_wait_enq(Handler handler) {
    this->service.async_wait_enq(this->implementation,q_,handler);
  }
private:
  std::shared_ptr<Queue>q_;
};
// typedefs for using standard queue listeners
template<typename Queue>using queue_sender=basic_queue_sender<basic_queue_sender_service<>,Queue>;

// --- service class -----------------------------
// (for one io_service, only one object created)
template<typename Impl>
class basic_queue_sender_service:public boost::asio::io_service::service{
public:
  // required to have id of service
 static boost::asio::io_service::id id;

  // ctor
  explicit basic_queue_sender_service(boost::asio::io_service&io_service):
      boost::asio::io_service::service(io_service){
  }
  // dtor
  ~basic_queue_sender_service(){
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
  // async sync enq operation
  template <typename Handler,typename Queue>
  void async_enq(implementation_type&impl,std::shared_ptr<Queue>q,typename Queue::value_type val,Handler handler){
    // this is a non-blocking operation so we are OK calling impl object in this thread
    impl->async_enq(impl,q,val,handler);
  }
  // sync enq operation (blocking)
  template <typename Queue>
  void sync_enq(implementation_type&impl,std::shared_ptr<Queue>q,typename Queue::value_type val){
    impl->sync_enq(q,val);
  }
  // async sync wait operation
  template <typename Handler,typename Queue>
  void async_wait_enq(implementation_type&impl,std::shared_ptr<Queue>q,Handler handler){
    // this is a non-blocking operation so we are OK calling impl object in this thread
    impl->async_wait_enq(impl,q,handler);
  }
private:
  // shutdown service (required)
  void shutdown_service(){
  }
};
// definition of id of service (required)
template <typename Impl>
boost::asio::io_service::id basic_queue_sender_service<Impl>::id;

// --- implementation -----------------------------
class queue_sender_impl{
public:
  // ctor (set up work queue for io_service so we don't bail out when executing run())
  queue_sender_impl(boost::asio::io_service&post_io_service):
      impl_work_(new boost::asio::io_service::work(impl_io_service_)),
      impl_thread_([&](){impl_io_service_.run();}),
      post_io_service_(post_io_service){
  }
  // dtor (clear work queue, stop io service and join thread)
  ~queue_sender_impl(){
    impl_work_.reset(nullptr);
    impl_io_service_.stop();
    if(impl_thread_.joinable())impl_thread_.join();
  }
public:
  // enque message (post request to thread)
  template<typename Handler,typename Queue>
  void async_enq(std::shared_ptr<queue_sender_impl>impl,std::shared_ptr<Queue>tq,typename Queue::value_type val,Handler handler){
    impl_io_service_.post(enq_operation<Handler,Queue>(impl,post_io_service_,tq,val,handler));
  }
  // enque message (blocking enq)
  template<typename Queue>
  void sync_enq(std::shared_ptr<Queue>tq,typename Queue::value_type val){
    tq->enq(val);
  }
  // wait to enq message (post request to thread)
  template<typename Handler,typename Queue>
  void async_wait_enq(std::shared_ptr<queue_sender_impl>impl,std::shared_ptr<Queue>tq,Handler handler){
    impl_io_service_.post(wait_enq_operation<Handler,Queue>(impl,post_io_service_,tq,handler));
  }
private:
  // function object calling blocking enq() on queue
  template <typename Handler,typename Queue>
  class enq_operation{
  public:
    // ctor
    enq_operation(std::shared_ptr<queue_sender_impl>impl,boost::asio::io_service &io_service,std::shared_ptr<Queue>tq,typename Queue::value_type val,Handler handler):
        wimpl_(impl),io_service_(io_service),work_(io_service),tq_(tq),val_(val),handler_(handler) {
    }
    // function calling implementation object - runs in the thread created in ctor
    void operator()(){
      // make sure implementation object is still valid
      std::shared_ptr<queue_sender_impl>impl{wimpl_.lock()};

      // if valid, go ahead and do (potentially) blocking call on queue, otherwise post aborted message
      if(impl){
        bool ret{tq_->enq(val_)};
        boost::system::error_code ec=(!ret?boost::asio::error::operation_aborted:boost::system::error_code());
        this->io_service_.post(boost::asio::detail::bind_handler(handler_,ec));
      }else{
        this->io_service_.post(boost::asio::detail::bind_handler(handler_,boost::asio::error::operation_aborted));
      }
    }
  private:
    std::weak_ptr<queue_sender_impl>wimpl_;
    boost::asio::io_service&io_service_;
    boost::asio::io_service::work work_;
    std::shared_ptr<Queue>tq_;
    typename Queue::value_type val_;
    Handler handler_;
  };
  // function object calling blocking wait() on queue
  template <typename Handler,typename Queue>
  class wait_enq_operation{
  public:
    // ctor
    wait_enq_operation(std::shared_ptr<queue_sender_impl>impl,boost::asio::io_service &io_service,std::shared_ptr<Queue>tq,Handler handler):
        wimpl_(impl),io_service_(io_service),work_(io_service),tq_(tq),handler_(handler) {
    }
    // function calling implementation object - runs in the thread created in ctor
    void operator()(){
      // make sure implementation object is still valid
      std::shared_ptr<queue_sender_impl>impl{wimpl_.lock()};

      // if valid, go ahead and do (potentially) blocking call on queue, otherwise post aborted message
      if(impl){
        bool ret{tq_->wait_enq()};
        boost::system::error_code ec=(!ret?boost::asio::error::operation_aborted:boost::system::error_code());
        this->io_service_.post(boost::asio::detail::bind_handler(handler_,ec));
      }else{
        this->io_service_.post(boost::asio::detail::bind_handler(handler_,boost::asio::error::operation_aborted));
      }
    }
  private:
    std::weak_ptr<queue_sender_impl>wimpl_;
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

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#ifndef QUEUE_INTERFACE_ENQ_H__
#define QUEUE_INTERFACE_ENQ_H__
#include "queue_empty_base.hpp"
#include <utility>
#include <boost/asio/error.hpp>

namespace boost{
namespace asio{
namespace detail{
namespace base{

// empty dummy class used for queues base class
template<typename T>
struct queue_interface_enq:public virtual queue_empty_base<T>{
  virtual~queue_interface_enq(){}

  // enq functions
  virtual bool enq(T t,boost::system::error_code&ec)=0;
  virtual bool timed_enq(T t,std::size_t ms,boost::system::error_code&ec)=0;
  virtual bool wait_enq(boost::system::error_code&ec)=0;
  virtual bool timed_wait_enq(std::size_t ms,boost::system::error_code&ec)=0;
  virtual void disable_enq(bool disable)=0;
};
}
}
}
}
#endif

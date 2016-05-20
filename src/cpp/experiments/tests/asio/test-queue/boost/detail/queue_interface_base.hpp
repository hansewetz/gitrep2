// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#ifndef QUEUE_INTERFACE_BASE_H__
#define QUEUE_INTERFACE_BASE_H__
#include "queue_interface_enq.hpp"
#include "queue_interface_deq.hpp"
namespace boost{
namespace asio{
namespace detail{
namespace base{

// empty dummy class used for queues base class
template<typename T>
struct queue_interface_base:public queue_interface_enq<T>,public queue_interface_deq<T>{
};
}
}
}
}
#endif

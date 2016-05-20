// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#ifndef QUEUE_EMPTY_BASE_H__
#define QUEUE_EMPTY_BASE_H__
namespace boost{
namespace asio{
namespace detail{
namespace base{

// empty dummy class used for queues base class
template<typename T>
struct queue_empty_base{
  using value_type=T;
};
}
}
}
}
#endif

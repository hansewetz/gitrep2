#ifndef __ASIO_QUEUE_H__
#define __ASIO_QUEUE_H__
#include "queue_listener.h"
#include "queue_sender.h"
#include "simple_queue.h"
#include "polldir_queue.h"
namespace boost{
namespace asio{

// typedefs for using standard queue listeners
template<typename Queue>using simple_queue_listener=basic_queue_listener<basic_queue_listener_service<>,simple_queue<typename Queue::value_type>>;
template<typename Queue>using polldir_queue_listener=basic_queue_listener<basic_queue_listener_service<>,polldir_queue<typename Queue::value_type,typename Queue::deserialiser,typename Queue::serialiser>>;

// typedefs for using standard queue senders
template<typename Queue>using simple_queue_sender=basic_queue_sender<basic_queue_sender_service<>,simple_queue<typename Queue::value_type>>;
template<typename Queue>using polldir_queue_sender=basic_queue_sender<basic_queue_sender_service<>,polldir_queue<typename Queue::value_type,typename Queue::deserialiser,typename Queue::serialiser>>;
}
}
#endif

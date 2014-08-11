#include "queue_listener.h"
#include "queue_sender.h"
#include <boost/asio.hpp>
#include <boost/asio/posix/basic_descriptor.hpp>
#include <boost/log/trivial.hpp>
#include <string>
#include <memory>
using namespace std;
using namespace std::placeholders;

// asio io_service
boost::asio::io_service ios;

// asio stuff
size_t maxmsg{3};
shared_ptr<boost::asio::simple_queue<string>>q{new boost::asio::simple_queue<string>(maxmsg)};
boost::asio::simple_queue_listener<string>qlistener(::ios,q);
boost::asio::simple_queue_sender<string>qsender(::ios,q);
boost::asio::deadline_timer timer(::ios,boost::posix_time::milliseconds(5000));

// count #of outstanding messages
size_t nmsg{0};

// handler for queue listener
void flisten(boost::system::error_code const&ec,string s){
  BOOST_LOG_TRIVIAL(debug)<<"dequed message";
  // reload listener only if we have more messages
  if(--nmsg>0)qlistener.async_deq(flisten);
}
// wait handler
void fwait(boost::system::error_code const&ec){
  BOOST_LOG_TRIVIAL(debug)<<"GOT WAIT MESSAGE";
  // reload listener only if we have more messages
  if(nmsg>0)qlistener.async_deq(flisten);
}
void ftimer(boost::system::error_code const&ec){
  BOOST_LOG_TRIVIAL(debug)<<"TICK - starting async read of 1 message ...";
  if(nmsg>0)qlistener.async_deq(flisten);
}
// sender handler
void fsender(boost::system::error_code const&ec){
  // if queue is full, then wait
  if(q->full()){
    BOOST_LOG_TRIVIAL(debug)<<"starting async_wait() ...";
    qsender.async_wait(fwait);
  }
}
// test program
int main(){
  try{
    // kick off a batch of async message enques
    string item{"Hello"};
    for(;nmsg<maxmsg;++nmsg){
      BOOST_LOG_TRIVIAL(debug)<<"enqueued message ...";
      qsender.async_enq(item,fsender);
    }
    // set timer for then start deq() so wait will trigger on queue
    BOOST_LOG_TRIVIAL(debug)<<"starting timer ...";
    timer.async_wait(ftimer);

    // run asio 
    ::ios.run();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

#include "queue_listener.h"
#include "queue_sender.h"
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <string>
#include <memory>
#include <thread>
using namespace std;
using namespace std::placeholders;

// max #of messages to send
constexpr size_t maxmsg{10};

// underlying queue
std::size_t maxqitems{3};
shared_ptr<boost::asio::simple_queue<int>>q{new boost::asio::simple_queue<int>(maxqitems)};

// asio io service
boost::asio::io_service ios;

// asio queue listener and sender
boost::asio::simple_queue_listener<int>qlistener(::ios);
boost::asio::simple_queue_sender<int>qsender(::ios);

// queue listener handler for queue 1
size_t nreceived1{0};
template<typename T>
void qlistener_handler(boost::system::error_code const&ec,T item,boost::asio::simple_queue_listener<T>*asioq,shared_ptr<boost::asio::simple_queue<int>>q){
  // print item if error code is OK
  if(ec)BOOST_LOG_TRIVIAL(debug)<<"received item in qlistener_handler (via asio), item: <invalid>, ec: "<<ec;
  else{
    BOOST_LOG_TRIVIAL(debug)<<"received item in qlistener_handler (via asio), item: "<<item<<", ec: "<<ec;
    if(++nreceived1!=maxmsg)asioq->async_deq(q,std::bind(qlistener_handler<T>,_1,_2,asioq,q));
  }
}
// queue sender handler for queue 1
size_t nsent1{0};
template<typename T>
void qsender_handler(boost::system::error_code const&ec,boost::asio::simple_queue_sender<T>*asioq,shared_ptr<boost::asio::simple_queue<int>>q,int item){
  // print item if error code is OK
  if(ec)BOOST_LOG_TRIVIAL(debug)<<"sent item in qsender_handler (via asio), item: <invalid>, ec: "<<ec;
  else{
    BOOST_LOG_TRIVIAL(debug)<<"sending item in qlistener_handler (via asio), item: "<<item<<", ec: "<<ec;
    if(++nsent1!=maxmsg)asioq->async_enq(q,item+1,std::bind(qsender_handler<T>,_1,asioq,q,item+1));
  }
}
// timer handler
void timer_handler(const boost::system::error_code&err){
  // start listening to messages
  qlistener.async_deq(q,std::bind(qlistener_handler<int>,_1,_2,&qlistener,q));
}
// test program
int main(){
  try{
    // setup time to trigger after 3 seconds after which we start listening to messages
    boost::asio::deadline_timer tmo(::ios,boost::posix_time::seconds(3));
    tmo.async_wait(timer_handler);

    // start sending messages
    qsender.async_enq(q,1,std::bind(qsender_handler<int>,_1,&qsender,q,1));

    // run io service and join sender thread
    ::ios.run();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

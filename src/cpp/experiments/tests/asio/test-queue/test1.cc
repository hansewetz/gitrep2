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

// underlying queue holding max 'maxqitems'
std::size_t maxqitems{3};
shared_ptr<boost::asio::simple_queue<int>>q{new boost::asio::simple_queue<int>(maxqitems)};

// asio objects
boost::asio::io_service ios;
boost::asio::simple_queue_listener<int>qlistener(::ios,q);
boost::asio::simple_queue_sender<int>qsender(::ios,q);
boost::asio::deadline_timer tmo(::ios,boost::posix_time::seconds(3));

// queue listener handler for queue
size_t nreceived{0};
template<typename T>
void qlistener_handler(boost::system::error_code const&ec,T item){
  // print item if error code is OK
  if(ec)BOOST_LOG_TRIVIAL(debug)<<"queue listener interupted (via asio): ignoring callback queue item, ec: "<<ec;
  else{
    BOOST_LOG_TRIVIAL(debug)<<"received item in qlistener_handler (via asio), item: "<<item<<", ec: "<<ec;
    if(++nreceived!=maxmsg)qlistener.async_deq(std::bind(qlistener_handler<T>,_1,_2));
  }
}
// queue sender handler for queue
size_t nsent{0};
template<typename T>
void qsender_handler(boost::system::error_code const&ec,int item){
  // print item if error code is OK
  if(ec)BOOST_LOG_TRIVIAL(debug)<<"queue sender interupted (via asio): ignoring callback, ec: "<<ec
  else{
    BOOST_LOG_TRIVIAL(debug)<<"sending item in qlistener_handler (via asio), item: "<<item<<", ec: "<<ec;
    if(++nsent!=maxmsg)qsender.async_enq(item+1,std::bind(qsender_handler<T>,_1,item+1));
  }
}
// timer handler
void timer_handler(const boost::system::error_code&err){
  // start listening to messages
  qlistener.async_deq(std::bind(qlistener_handler<int>,_1,_2));
}
// test program
int main(){
  try{
    // setup timer to trigger after which we start listening to messages
    tmo.async_wait(timer_handler);

    // start sending messages
    int startItem{1};
    qsender.async_enq(startItem,std::bind(qsender_handler<int>,_1,startItem));

    // run io service and join sender thread
    ::ios.run();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

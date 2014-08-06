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

// ----------------- queue sending/receiving max #of messages
// asio queue stuff
shared_ptr<boost::asio::simple_queue<int>>q{new boost::asio::simple_queue<int>(3)};
boost::asio::io_service ios;
boost::asio::simple_queue_listener<int>qlistener(::ios,q);
boost::asio::simple_queue_sender<int>qsender(::ios,q);

// max #of messages to send/receive
constexpr size_t maxmsg{10};

// handler for queue listener
template<typename T>
void qlistener_handler(boost::system::error_code const&ec,T item, int nreceived){
  // print item if error code is OK
  if(ec)BOOST_LOG_TRIVIAL(debug)<<"queue listener interupted (via asio): ignoring callback queue item, ec: "<<ec;
  else{
    BOOST_LOG_TRIVIAL(debug)<<"received item in qlistener_handler (via asio), item: "<<item<<", ec: "<<ec;
    if(nreceived+1<maxmsg)qlistener.async_deq(std::bind(qlistener_handler<T>,_1,_2,nreceived+1));
  }
}
// handler for queue sender
template<typename T>
void qsender_handler(boost::system::error_code const&ec,int item,int nsent){
  // print item if error code is OK
  if(ec)BOOST_LOG_TRIVIAL(debug)<<"queue sender interupted (via asio): ignoring callback, ec: "<<ec;
  else{
    BOOST_LOG_TRIVIAL(debug)<<"sending item in qlistener_handler (via asio), item: "<<item<<", ec: "<<ec;
    if(nsent+1<maxmsg)qsender.async_enq(item+1,std::bind(qsender_handler<T>,_1,item+1,nsent+1));
  }
}

// ----------------- queue (q1) sending/receiving until a timer pops
//  asio queue stuff
shared_ptr<boost::asio::simple_queue<int>>q1{new boost::asio::simple_queue<int>(1000)};
boost::asio::simple_queue_listener<int>qlistener1(::ios,q1);
boost::asio::simple_queue_sender<int>qsender1(::ios,q1);

// handler for queue listener
template<typename T>
void qlistener1_handler(boost::system::error_code const&ec,T item){
  if(ec)BOOST_LOG_TRIVIAL(debug)<<"queue listener1 interupted (via asio): ignoring callback queue item, ec: "<<ec;
  else{
    BOOST_LOG_TRIVIAL(debug)<<"received item in qlistener1_handler (via asio), item: "<<item<<", ec: "<<ec;
    qlistener1.async_deq(std::bind(qlistener1_handler<T>,_1,_2));
  }
}
// handler for queue sender
template<typename T>
void qsender1_handler(boost::system::error_code const&ec,int item){
  // print item if error code is OK
  if(ec)BOOST_LOG_TRIVIAL(debug)<<"queue sender1 interupted (via asio): ignoring callback, ec: "<<ec;
  else{
    BOOST_LOG_TRIVIAL(debug)<<"sending item in qlistener1_handler (via asio), item: "<<item<<", ec: "<<ec;
    qsender1.async_enq(item+1,std::bind(qsender1_handler<T>,_1,item+1));
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
  }
}

// test program
int main(){
  try{
    // --------------- setup queue which sends/receives a specific #if messages

    // setup timer to trigger in 3 seconds after which we start listening to messages
    boost::asio::deadline_timer tmo(::ios,boost::posix_time::seconds(3));
    tmo.async_wait([&](const boost::system::error_code&ec){qlistener.async_deq(std::bind(qlistener_handler<int>,_1,_2,0));});

    // start sending messages
    int startItem{1};
    qsender.async_enq(startItem,std::bind(qsender_handler<int>,_1,startItem,0));

    // --------------- setup queue which sends/receives messageses until a timer pops

    // setup timer for test - when timer pops, we disable enq and deq
    boost::asio::deadline_timer tmo1(::ios,boost::posix_time::seconds(4));
    tmo1.async_wait([&](const boost::system::error_code&ec){q1->disable_enq(true);q1->disable_deq(true);});

    // listen/send on messages on q1
    qlistener1.async_deq(std::bind(qlistener1_handler<int>,_1,_2));
    qsender1.async_enq(100,std::bind(qsender1_handler<int>,_1,100));

    // --------------- kick off io service
    ::ios.run();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

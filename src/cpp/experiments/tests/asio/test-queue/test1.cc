#include "simple_queue.h"
#include "queue_listener.h"
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>
#include <memory>
#include <thread>
using namespace std;
using namespace std::placeholders;

// some constants
constexpr size_t maxmsg1{3};
constexpr size_t tmoSeleepBetweenSendMs1{100};

// queue listener handler for queue 1
size_t nreceived1{0};
template<typename T>
void qhandler1(boost::system::error_code const&ec,T item,boost::asio::simple_queue_listener<T>*asioq,shared_ptr<boost::asio::simple_queue<string>>q1){
  // print item if error code is OK
  if(ec)BOOST_LOG_TRIVIAL(debug)<<"received item in qhandler1 (via asio), item: <invalid>, ec: "<<ec;
  else{
   BOOST_LOG_TRIVIAL(debug)<<"received item in qhandler1 (via asio), item: "<<item<<", ec: "<<ec;
    asioq->async_deq(q1,std::bind(qhandler1<T>,_1,_2,asioq,q1));
  }
}
// queue sender for queue 1
size_t nsent{0};
void senderq1(shared_ptr<boost::asio::simple_queue<string>>q1){
  for(int i=0;i<maxmsg1;++i){
    string item{boost::lexical_cast<string>(i)};
    BOOST_LOG_TRIVIAL(debug)<<"sending item \""<<item<<"\"in separate thread ...";
    q1->enq(item);
    this_thread::sleep_for(std::chrono::milliseconds(tmoSeleepBetweenSendMs1));
  }
  // disable dequeing (will stop async listener)
  q1->disable_deq(true);
}
// test program
int main(){
  try{
    // underlying queue
    shared_ptr<boost::asio::simple_queue<string>>q1{new boost::asio::simple_queue<string>};

    // asio io service
    boost::asio::io_service ios;

    // asio queue listeners
    boost::asio::simple_queue_listener<string>qlistener1(ios);
    qlistener1.async_deq(q1,std::bind(qhandler1<string>,_1,_2,&qlistener1,q1));

    // run a sender thread, run io service and join sender thread
    std::thread thrq1{senderq1,q1};
    ios.run();
    thrq1.join();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}

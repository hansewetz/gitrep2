#include "simple_queue.h"
#include "queue_listener.h"
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>
#include <memory>
#include <thread>
#include <functional>
using namespace std;
using namespace std::placeholders;

// some constants
constexpr size_t maxmsg1{1000000};
constexpr size_t tmoSeleepBetweenSendMs1{0};

// queue listener handler for queue 1
size_t nreceived1{0};
template<typename T>
void qhandler1(boost::system::error_code const&ec,T item,boost::asio::simple_queue_listener<T>*asioq,shared_ptr<boost::asio::simple_queue<string>>q1,bool bailout){
  // print item if error code is OK
  if(ec)BOOST_LOG_TRIVIAL(debug)<<"received item in qhandler1 (via asio), item: <invalid>, ec: "<<ec;
  else BOOST_LOG_TRIVIAL(debug)<<"received item in qhandler1 (via asio), item: "<<item<<", ec: "<<ec;

  // check if we should reload IO object
  if(++nreceived1>=maxmsg1){
    BOOST_LOG_TRIVIAL(debug)<<"have received eneough messages in qhandler1";
    if(bailout)q1->disable_deq(true);
  }else{
    asioq->async_deq(q1,std::bind(qhandler1<T>,_1,_2,asioq,q1,bailout));
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
}
// test program
int main(){
  try{
    // underlying queue
    shared_ptr<boost::asio::simple_queue<string>>q1{new boost::asio::simple_queue<string>};

    // asio io service
    boost::asio::io_service ios;

    // bool specifying if queue handler should cancel deq() operations on queue when it has received all messages it needs
    bool q1bailout{true};

    // asio queue listeners
    boost::asio::simple_queue_listener<string>qlistener1(ios);
    qlistener1.async_deq(q1,std::bind(qhandler1<string>,_1,_2,&qlistener1,q1,q1bailout));
    boost::asio::simple_queue_listener<string>qlistener2(ios);
    qlistener2.async_deq(q1,std::bind(qhandler1<string>,_1,_2,&qlistener2,q1,q1bailout));

    // run a sender thread
    std::thread thrq1{senderq1,q1};

    // run io service
    ios.run();

    // join sender thread
    thrq1.join();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}

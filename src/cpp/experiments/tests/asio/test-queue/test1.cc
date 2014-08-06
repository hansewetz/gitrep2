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

// asio queue stuff
boost::asio::io_service ios;
boost::asio::simple_queue_listener<int>qlistener(::ios,q);
boost::asio::simple_queue_sender<int>qsender(::ios,q);

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
// test program
int main(){
  try{
    // setup timer to trigger after 3 seconds aftr which we start listening to messages
    boost::asio::deadline_timer tmo(::ios,boost::posix_time::seconds(3));
    tmo.async_wait([&](const boost::system::error_code&ec){qlistener.async_deq(std::bind(qlistener_handler<int>,_1,_2,0));});

    // start sending messages
    int startItem{1};
    qsender.async_enq(startItem,std::bind(qsender_handler<int>,_1,startItem,0));

    // run io service and join sender thread
    ::ios.run();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

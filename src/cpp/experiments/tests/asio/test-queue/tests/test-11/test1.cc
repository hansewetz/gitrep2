/*
This program tests sync_deq()
*/

#include <boost/asio_queue.h>
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
using queue_t=boost::asio::simple_queue<int>;
shared_ptr<queue_t>q{new queue_t(3)};
boost::asio::io_service ios;
boost::asio::queue_listener<queue_t>qlistener(::ios,q.get());
boost::asio::queue_sender<queue_t>qsender(::ios,q.get());

// max #of messages to send/receive
constexpr size_t maxmsg{10};

// sender function
void sender(){
  for(int i=0;i<maxmsg;++i){
    qsender.sync_enq(i);
  }
}
// receiver function
void receiver(){
  for(int i=0;i<maxmsg;++i){
    auto p=qlistener.sync_deq() ;
    cerr<<"received: ["<<boolalpha<<p.first<<", "<<p.second<<"]"<<endl;
  }
}
// test program
int main(){
  try{
    // kick off sender thread
    thread tsender{sender};

    // kick off listener thread
    thread treceiver{receiver};

    // kick off asio
    ::ios.run();

    // join threads
    tsender.join();
    treceiver.join();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

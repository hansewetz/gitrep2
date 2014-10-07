/*
this program has one queue with a thread doing sync_enq and an asio in main thread doing async_deq
this is the same program as test4.cc but using a polldir_queue instead of a simple_queue

*/

#include "asio_queue.h"
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <memory>
#include <thread>
#include <functional>
using namespace std;
using namespace std::placeholders;

namespace asio= boost::asio;
namespace fs=boost::filesystem;

// create queue
function<int(istream&)>deserialiser=[](istream&is){int ret;is>>ret;return ret;};
function<void(ostream&,int)>serialiser=[](ostream&os,int i){os<<i;};
fs::path qdir{"./q1"};
using intq_t=asio::polldir_queue<int,decltype(deserialiser),decltype(serialiser)>;
std::shared_ptr<intq_t>q{new intq_t(10,5000,qdir,deserialiser,serialiser,true)};

// setup asio object
asio::io_service ios;
asio::polldir_queue_listener<intq_t>qlistener(::ios,q);
asio::polldir_queue_sender<intq_t>qsender(::ios,q);

// max #of messages to send/receive
constexpr size_t maxmsg{10};

// handler for queue listener
size_t nreceived{0};
template<typename T>
void qlistener_handler(boost::system::error_code const&ec,T item){
  BOOST_LOG_TRIVIAL(debug)<<"received item in qlistener_handler (via asio), item: "<<item<<", ec: "<<ec;
  if(++nreceived<maxmsg)qlistener.async_deq(qlistener_handler<T>);
}
// thread function sending maxmsg messages
void thr_send_sync_messages(){
  for(int i=0;i<maxmsg;++i){
    int item{boost::lexical_cast<int>(i)};
    BOOST_LOG_TRIVIAL(debug)<<"sending item: "<<item;
    qsender.sync_enq(item);
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
  }
}
// test program
int main(){
  try{
    // listen/send on messages on q1
    qlistener.async_deq(qlistener_handler<int>);

    // kick off sender thread
    thread thr(thr_send_sync_messages);

    // kick off io service
    ::ios.run();

    // join thread
    thr.join();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

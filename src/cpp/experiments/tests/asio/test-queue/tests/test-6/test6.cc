/*
this program has one queue with a thread doing sync_enq and an asio in main thread doing async_deq
this is the same program as test4.cc but using a polldir_queue instead of a simple_queue

*/

#include <boost/asio_queue.h>
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
std::shared_ptr<intq_t>q{new intq_t(0,50,qdir,deserialiser,serialiser,true)};

// setup asio object
asio::io_service ios;
asio::polldir_queue_listener<intq_t>qlistener(::ios,q);
asio::polldir_queue_sender<intq_t>qsender(::ios,q);

// timer
boost::asio::deadline_timer timer(::ios,boost::posix_time::milliseconds(5000));

// max #of messages to send/receive
constexpr size_t maxmsg{10};

// handler for queue listener
template<typename T>
void qlistener_handler(boost::system::error_code const&ec,T item){
  if(ec==boost::asio::error::operation_aborted){
    BOOST_LOG_TRIVIAL(debug)<<"deque() aborted (via asio), ec: "<<ec;
  }else
  if(ec.value()!=0){
    BOOST_LOG_TRIVIAL(debug)<<"deque() error (via asio), ec: "<<ec;
  }else{
    BOOST_LOG_TRIVIAL(debug)<<"received item in qlistener_handler (via asio), item: "<<item<<", ec: "<<ec;
    qlistener.async_deq(qlistener_handler<T>);
  }
}
// thread function sending maxmsg messages
void thr_send_sync_messages(){
  for(int i=0;i<maxmsg;++i){
    BOOST_LOG_TRIVIAL(debug)<<"sending item: "<<i;
    qsender.sync_enq(2*i);
    qsender.sync_enq(2*i+1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
// timer for killing dequeue
void stopTimer(boost::system::error_code const&ec){
  BOOST_LOG_TRIVIAL(debug)<<"TICK - stopping deque ...";
  q->disable_deq(true);
}
// test program
int main(){
  try{
    // remove locks for queue
    q->removeLockVariables(qdir);

    // listen/send on messages on q1
    BOOST_LOG_TRIVIAL(debug)<<"starting async_deq() ...";
    qlistener.async_deq(qlistener_handler<int>);

    // kick off sender thread
    BOOST_LOG_TRIVIAL(debug)<<"starting thread sender thread ...";
    thread thr(thr_send_sync_messages);

    // set timer stopping dequeing on q
    BOOST_LOG_TRIVIAL(debug)<<"starting timer ...";
    timer.async_wait(stopTimer);

    // kick off io service
    BOOST_LOG_TRIVIAL(debug)<<"starting asio ...";
    ::ios.run();

    // join thread
    BOOST_LOG_TRIVIAL(debug)<<"joining with sender thread ...";
    thr.join();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

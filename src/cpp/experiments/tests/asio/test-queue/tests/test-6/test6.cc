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

// value type in queues
// (must work with operator<< and operator>>, and be default constructable)
using qval_t=std::string;

// create a sender and listener queues (could be the same queue)
function<qval_t(istream&)>deserialiser=[](istream&is){qval_t ret;is>>ret;return ret;};
function<void(ostream&,qval_t const&)>serialiser=[](ostream&os,qval_t const&i){os<<i;};
string qname{"q1"};
fs::path qdir{"./q1"};
using queue_t=asio::polldir_queue<qval_t,decltype(deserialiser),decltype(serialiser)>;
std::shared_ptr<queue_t>qrecv{new queue_t(qname,0,qdir,deserialiser,serialiser,true)};
std::shared_ptr<queue_t>qsend{new queue_t(qname,0,qdir,deserialiser,serialiser,true)};

// setup asio object
asio::io_service ios;
asio::queue_listener<queue_t>qlistener(::ios,qrecv);
asio::queue_sender<queue_t>qsender(::ios,qsend);

// timer - when popping it stops deq()
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
template<typename T>
void thr_send_sync_messages(){
  for(int i=0;i<maxmsg;++i){
    qval_t item{boost::lexical_cast<qval_t>(i)};
    BOOST_LOG_TRIVIAL(debug)<<"sending item: "<<item;
    qsender.sync_enq(item);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
// timer for killing dequeue
void stopTimer(boost::system::error_code const&ec){
  BOOST_LOG_TRIVIAL(debug)<<"TICK - stopping deque ...";
  qrecv->disable_deq(true);
}
// test program
int main(){
  try{
    // remove locks for queue
    qrecv->removeLockVariables(qrecv->qname());

    // listen for on messages on q1 (using asio)
    BOOST_LOG_TRIVIAL(debug)<<"starting async_deq() ...";
    qlistener.async_deq(qlistener_handler<qval_t>);

    // set timer stopping dequeing on q
    // (at some point we have to stop deq() or we'll sit in asio forever)
    BOOST_LOG_TRIVIAL(debug)<<"starting timer ...";
    timer.async_wait(stopTimer);

    // kick off sender thread
    BOOST_LOG_TRIVIAL(debug)<<"starting thread sender thread ...";
    thread thr(thr_send_sync_messages<qval_t>);

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

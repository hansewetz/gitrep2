/*
this program has one queue with a thread doing sync_enq and an asio in main thread doing async_deq
this is the same program as test4.cc but using a polldir_queue instead of a simple_queue

*/

#include <boost/asio_queue.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <memory>
#include <thread>
#include <functional>
#include <iostream>
using namespace std;
using namespace std::placeholders;

namespace asio= boost::asio;
namespace fs=boost::filesystem;

// test queue item
class Junk{
public:
  friend ostream&operator<<(ostream&os,Junk const&junk){
    os<<junk.s_<<" "<<junk.i_;
  }
  friend istream&operator>>(istream&is,Junk&junk){
    is>>junk.s_>>junk.i_;
  }
  Junk()=default;
  Junk(string const&s,int i):s_(s),i_(i){}
  Junk(Junk const&)=default;
  Junk(Junk&&)=default;
  Junk&operator=(Junk const&)=default;
  Junk&operator=(Junk&&)=default;
private:
  string s_;
  int i_;
};
// timeout for deq()
size_t tmo_ms{3000};

// value type in queues
// (must work with operator<< and operator>>, and be default constructable)
using qval_t=Junk;

// create a sender and listener queues (could be the same queue)
function<qval_t(istream&)>deserialiser=[](istream&is){qval_t ret;is>>ret;return ret;};
function<void(ostream&,qval_t const&)>serialiser=[](ostream&os,qval_t const&i){os<<i;};
string qname{"q1"};
fs::path qdir{"./q1"};
using queue_t=asio::polldir_queue<qval_t,decltype(deserialiser),decltype(serialiser)>;
queue_t qrecv{qname,0,qdir,deserialiser,serialiser,true};
queue_t qsend{qname,0,qdir,deserialiser,serialiser,true};

// setup asio object
asio::io_service ios;
asio::queue_listener<queue_t>qlistener(::ios,&qrecv);
asio::queue_sender<queue_t>qsender(::ios,&qsend);

// max #of messages to send/receive
constexpr size_t maxmsg{10};

// handler for queue listener
template<typename T>
void qlistener_handler(boost::system::error_code const&ec,T item){
  if(ec!=0){
    BOOST_LOG_TRIVIAL(debug)<<"deque() aborted (via asio), ec: "<<ec.message();
  }else{
    BOOST_LOG_TRIVIAL(debug)<<"received item in qlistener_handler (via asio), item: "<<item<<", ec: "<<ec;
    qlistener.timed_async_deq(qlistener_handler<T>,tmo_ms);
  }
}
// thread function sending maxmsg messages
template<typename T>
void thr_send_sync_messages(){
  for(int i=0;i<maxmsg;++i){
    qval_t item{Junk{string("Hello")+boost::lexical_cast<string>(i),i}};
    BOOST_LOG_TRIVIAL(debug)<<"sending item: "<<item;
    boost::system::error_code ec;
    qsender.sync_enq(item,ec);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
// test program
int main(){
  try{
    // remove locks for queue
    qrecv.removeLockVariables(qrecv.qname());

    // listen for on messages on q1 (using asio)
    BOOST_LOG_TRIVIAL(debug)<<"starting async_deq() ...";
    qlistener.timed_async_deq(qlistener_handler<qval_t>,tmo_ms);

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

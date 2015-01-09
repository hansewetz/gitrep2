// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <boost/asio_queue.hpp>
#include <boost/log/trivial.hpp>
#include <string>
#include <thread>
#include <iostream>
#include <functional>
#include <memory>
#include <unistd.h>

using namespace std;
using namespace std::placeholders;
namespace asio=boost::asio;

// controll if client or server is sender/receiver
#define SERVER_SENDER true

// ----- some constants -----
namespace {
size_t msgcount{0};
constexpr size_t maxmsg{10};
constexpr size_t tmo_deq_ms{2000};
constexpr size_t tmo_enq_ms{1000};
constexpr size_t tmo_between_send{10};
}
// server debug
constexpr int listenport=7787;
string const server{"localhost"};

// ----- queue types -----

// aios io service
asio::io_service ios;

// queue type
using qval_t=string;

// serialize an object (notice: message boundaries are on '\n' characters)
std::function<void(std::ostream&,qval_t const&)>serialiser=[](std::ostream&os,qval_t const&s){
  os<<s;
};
// de-serialize an object (notice: message boundaries are on '\n' characters)
std::function<qval_t(istream&)>deserialiser=[](istream&is){
  string line;
  getline(is,line);
  return line;
};
// queue types
#ifdef SERVER_SENDER
using enq_t=asio::sockserv_queue<qval_t,decltype(deserialiser),decltype(serialiser)>;
using deq_t=asio::sockclient_queue<qval_t,decltype(deserialiser),decltype(serialiser)>;
#else
using deq_t=asio::sockserv_queue<qval_t,decltype(deserialiser),decltype(serialiser)>;
using enq_t=asio::sockclient_queue<qval_t,decltype(deserialiser),decltype(serialiser)>;
#endif

//  ------ asio objects, sender, callback handler etc. ---
// handler for queue listener
template<typename T>
void qlistener_handler(boost::system::error_code const&ec,T msg,asio::queue_listener<deq_t>*ql){
  if(ec!=0){
    BOOST_LOG_TRIVIAL(debug)<<"deque() aborted (via asio), ec: "<<ec.message();
  }else{
    BOOST_LOG_TRIVIAL(debug)<<"received msg in qlistener_handler (via asio), msg: \""<<msg<<"\", ec: "<<ec;
    ql->timed_async_deq(std::bind(qlistener_handler<T>,_1,_2,ql),tmo_deq_ms);
  }
}
// handler for waiting for starting to listen to messages
template<typename T>
void qlistener_waiter_handler(boost::system::error_code const&ec,asio::queue_listener<deq_t>*ql){
  if(ec!=0){
    BOOST_LOG_TRIVIAL(debug)<<"deque-wait() aborted (via asio), ec: "<<ec.message();
  }else{
    BOOST_LOG_TRIVIAL(debug)<<"an asio message waiting ...";
    ql->timed_async_deq(std::bind(qlistener_handler<T>,_1,_2,ql),tmo_deq_ms);
  }
}
// handler for queue sender
void qsender_handler(boost::system::error_code const&ec,asio::queue_sender<enq_t>*qs){
  // print item if error code is OK
  if(ec)BOOST_LOG_TRIVIAL(debug)<<"queue sender interupted (via asio): ignoring callback, ec: "<<ec;
  else{
    // check if we are done
    if(msgcount==maxmsg)return;

    // sent next message asynchrounously
    qval_t newmsg{boost::lexical_cast<string>(msgcount++)};
    BOOST_LOG_TRIVIAL(debug)<<"sending message: \""<<newmsg<<"\"";
    qs->timed_async_enq(newmsg,std::bind(qsender_handler,_1,qs),tmo_enq_ms);
    std::this_thread::sleep_for(std::chrono::milliseconds(tmo_between_send));
  }
}
// handler for waiting for starting sending messages
void qsender_waiter_handler(boost::system::error_code const&ec,asio::queue_sender<enq_t>*qs){
  if(ec!=0){
    BOOST_LOG_TRIVIAL(debug)<<"enq-wait() aborted (via asio), ec: "<<ec.message();
  }else{
    BOOST_LOG_TRIVIAL(debug)<<"it's now possible to send messages ...";

    // kick off async message sender
    qval_t newmsg{boost::lexical_cast<string>(msgcount++)};
    BOOST_LOG_TRIVIAL(debug)<<"sending message: \""<<newmsg<<"\"";
    qs->timed_async_enq(newmsg,std::bind(qsender_handler,_1,qs),tmo_enq_ms);
  }
}
// ------ test program
int main(){
  try{
    // create queues
    asio::sockclient_queue<qval_t,decltype(deserialiser),decltype(serialiser)>qclient(server,listenport,deserialiser,serialiser);
    BOOST_LOG_TRIVIAL(debug)<<"client queue created ...";
    asio::sockserv_queue<qval_t,decltype(deserialiser),decltype(serialiser)>qserv(listenport,deserialiser,serialiser);
    BOOST_LOG_TRIVIAL(debug)<<"server queue created ...";

    // setup asio object
#ifdef SERVER_SENDER
    asio::queue_sender<enq_t>qsender(::ios,&qserv);
    asio::queue_listener<deq_t>qlistener(::ios,&qclient);
#else
    asio::queue_listener<deq_t>qlistener(::ios,&qserv);
    asio::queue_sender<enq_t>qsender(::ios,&qclient);
#endif
    // wait tmo_enq_ms ms until we can send a message
    qval_t msg{boost::lexical_cast<string>(msgcount++)};
    BOOST_LOG_TRIVIAL(debug)<<"waiting until we can send messages ... ";
    qsender.timed_async_wait_enq(std::bind(qsender_waiter_handler,_1,&qsender),tmo_enq_ms);

    // wait for at most tmo_deq_ms ms for a message to become available
    BOOST_LOG_TRIVIAL(debug)<<"starting waiting for asio message ...";
    qlistener.timed_async_wait_deq(std::bind(qlistener_waiter_handler<qval_t>,_1,&qlistener),tmo_deq_ms);

    BOOST_LOG_TRIVIAL(debug)<<"starting asio ...";
    ::ios.run();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

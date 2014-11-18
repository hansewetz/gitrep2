#include <boost/asio_queue.hpp>
#include <boost/log/trivial.hpp>
#include <string>
#include <iostream>
#include <functional>
#include <memory>
#include <thread>
#include <unistd.h>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

using namespace std;
using namespace std::placeholders;
namespace asio=boost::asio;
namespace io=boost::iostreams;

// ----- queue types -----

// aios io service
asio::io_service ios;

// queue type
using qval_t=string;

// serialize an object (notice: message boundaries are on '\n' characters)
std::function<void(ostream&,qval_t const&)>serialiser=[](ostream&os,qval_t const&s){
  os<<s;
};
// de-serialize an object (notice: message boundaries are on '\n' characters)
std::function<qval_t(istream&)>deserialiser=[](istream&is){
  string line;
  getline(is,line);
  return line;
};
// queue types
using enq_t=asio::fdenq_queue<qval_t,decltype(serialiser)>;
using deq_t=asio::fddeq_queue<qval_t,decltype(deserialiser)>;

//  ------ asio objects, sender, callback handler etc. ---

// some constants
constexpr size_t maxmsg{10};
constexpr size_t tmo_ms{2000};

// handler for queue listener
template<typename T>
void qlistener_handler(boost::system::error_code const&ec,T item,asio::queue_listener<deq_t>*ql){
  if(ec!=0){
    BOOST_LOG_TRIVIAL(debug)<<"deque() aborted (via asio), ec: "<<ec.message();
  }else{
    BOOST_LOG_TRIVIAL(debug)<<"received item in qlistener_handler (via asio), item: "<<item<<", ec: "<<ec;
    ql->timed_async_deq(std::bind(qlistener_handler<T>,_1,_2,ql),tmo_ms);
  }
}
// handler for waiting for queue message
template<typename T>
void qlistener_waiter_handler(boost::system::error_code const&ec,asio::queue_listener<deq_t>*ql){
  if(ec!=0){
    BOOST_LOG_TRIVIAL(debug)<<"deque() aborted (via asio), ec: "<<ec.message();
  }else{
    BOOST_LOG_TRIVIAL(debug)<<"an asio message waiting ...";
    ql->timed_async_deq(std::bind(qlistener_handler<T>,_1,_2,ql),tmo_ms);
  }
}
// thread function sending maxmsg messages
void thr_send_sync_messages(asio::queue_sender<enq_t>*qs,enq_t*q){
  // wait and send messages for 1 second
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // send messages
  for(int i=0;i<maxmsg;++i){
    qval_t item{string{"Message: "}+boost::lexical_cast<string>(i)};
    BOOST_LOG_TRIVIAL(debug)<<"sending item: "<<item;
    boost::system::error_code ec;
    qs->sync_enq(item,ec);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

// ------ test program
int main(){
  try{
    // create an fd-pipe with connected read/write fds
    int fd[2];
    if(pipe(fd)!=0){
      BOOST_LOG_TRIVIAL(error)<<"pipe failed";
      exit(1);
    }
    int fdread{fd[0]};
    int fdwrite{fd[1]};

    // create queues
    boost::system::error_code ec;
    enq_t qin{fdwrite,serialiser};
    deq_t qout{fdread,deserialiser};

    // setup asio object
    asio::queue_sender<enq_t>qsender(::ios,&qin);
    asio::queue_listener<deq_t>qlistener(::ios,&qout);

    // wait until a message is available for at most 100ms
    BOOST_LOG_TRIVIAL(debug)<<"starting waiting for asio message ...";
    qlistener.timed_async_wait_deq(std::bind(qlistener_waiter_handler<qval_t>,_1,&qlistener),1100);

    // kick off sender thread
    BOOST_LOG_TRIVIAL(debug)<<"starting thread sender thread ...";
    std::thread thr(thr_send_sync_messages,&qsender,&qin);

    // kick off io service
    BOOST_LOG_TRIVIAL(debug)<<"starting asio ...";
    ::ios.run();

    // join thread
    BOOST_LOG_TRIVIAL(debug)<<"joining thread ...";
    if(thr.joinable())thr.join();

    // cleanup
    BOOST_LOG_TRIVIAL(debug)<<"closing fds ...";
    close(fdwrite);
    close(fdread);
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

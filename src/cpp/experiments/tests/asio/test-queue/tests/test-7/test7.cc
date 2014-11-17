#include <boost/asio_queue.hpp>
#include <boost/log/trivial.hpp>
#include <string>
#include <iostream>
#include <functional>
#include <memory>
#include <unistd.h>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

using namespace std;
namespace asio=boost::asio;
namespace io=boost::iostreams;

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
  while(true){
    string line;
    getline(is,line);
    if(line=="")break;
    return line;
  }
};
// queue types
using enq_t=asio::fdenq_queue<qval_t,decltype(serialiser)>;
using deq_t=asio::fddeq_queue<qval_t,decltype(deserialiser)>;

// handler for queue listener
template<typename T>
void qlistener_handler(boost::system::error_code const&ec,T item){
  if(ec!=0){
    BOOST_LOG_TRIVIAL(debug)<<"deque() aborted (via asio), ec: "<<ec.message();
  }else{
    BOOST_LOG_TRIVIAL(debug)<<"received item in qlistener_handler (via asio), item: "<<item<<", ec: "<<ec;
  }
}

// test program
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

    // listen for on messages on q1 (using asio)
    BOOST_LOG_TRIVIAL(debug)<<"starting async_deq() ...";
    qlistener.async_deq(qlistener_handler<qval_t>);

    // send a message
    qval_t msg{"Hello world"};
    qsender.sync_enq(msg,ec);

    // kick off io service
    BOOST_LOG_TRIVIAL(debug)<<"starting asio ...";
    ::ios.run();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

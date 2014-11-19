/*
This program:
	- setup of queue_sender<> and queue_listener<> connected to child process
		- creates a 2 pipes and spawns a child process connecting the two
		- stdin/stdout in the child are duped to the pipe ends on the child side
		- an asio::fdenq_queue is created from the write fd on the parent side
		- an asio::fddeq_queue is created from the read fd on the parent side
		- an asio async sender (queue_sender<>) is created from the sender queue
		- an asio async listener (queue_listener<>) is created from the sender queue
	- using the fdenq_queue<> a 'maxmsg' number of strings is sent aynchronously to the child process (chich in this case is '/bin/cat')
	- using the fddeq_queue<> messages are collected back asynchronoulsy from the child process
	- ones the fddeq_queue<> object times out it gets interupted and doe snot re-trigger listening which in effect casues a return from ::ios::run()
*/
#include "asio-extensions/asio_queue.hpp"
#include "utils/sysUtils.h"
#include "utils/logUtils.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

using namespace std;
using namespace utils;
using namespace std::placeholders;
namespace asio=boost::asio;

// ----- some constants -----
namespace {
size_t msgcount{0};
constexpr size_t maxmsg{20};
constexpr size_t tmo_deq_ms{2000};
constexpr size_t tmo_enq_ms{1000};
}
// io_service object
 boost::asio::io_service ios;

// serialize an object (notice: message boundaries are on '\n' characters)
std::function<void(std::ostream&,string const&)>serialiser=[](std::ostream&os,string const&s){
  os<<s;
};
// de-serialize an object (notice: message boundaries are on '\n' characters)
std::function<string(istream&)>deserialiser=[](istream&is){
  string line;
  getline(is,line);
  return line;
};
// queue types
using enq_t=asio::fdenq_queue<string,decltype(serialiser)>;
using deq_t=asio::fddeq_queue<string,decltype(deserialiser)>;

// handler for queue listener
void qlistener_handler(boost::system::error_code const&ec,string msg,asio::queue_listener<deq_t>*ql){
  if(ec!=0){
    BOOST_LOG_TRIVIAL(info)<<"deque() aborted (via asio), ec: "<<ec.message();
  }else{
    BOOST_LOG_TRIVIAL(info)<<"received msg in qlistener_handler (via asio), msg: \""<<msg<<"\", ec: "<<ec;
    ql->timed_async_deq(std::bind(qlistener_handler,_1,_2,ql),tmo_deq_ms);
  }
}
// handler for queue sender
void qsender_handler(boost::system::error_code const&ec,asio::queue_sender<enq_t>*qs){
  // print item if error code is OK
  if(ec)BOOST_LOG_TRIVIAL(info)<<"queue sender interupted (via asio): ignoring callback, ec: "<<ec;
  else{
    // check if we are done
    if(msgcount==maxmsg)return;

    // sent next message asynchrounously
    string msg{boost::lexical_cast<string>(msgcount++)};
    qs->timed_async_enq(msg,std::bind(qsender_handler,_1,qs),tmo_enq_ms);
  }
}
// main for test program
int main(){
  try{
    // set log level (true: log debug info, false: do not log debug info)
    utils::initBoostFileLogging(false);

    // spawn '/bin/cat' as child process
    int fdread,fdwrite;
    int cpid=spawnPipeChild("/bin/cat",vector<string>{"cat"},fdread,fdwrite,true);

    // create queues
    boost::system::error_code ec;
    enq_t qsend{fdwrite,serialiser};
    deq_t qrecv{fdread,deserialiser};
    
    // setup queue sender/listener
    asio::queue_sender<enq_t>qsender(::ios,&qsend);
    asio::queue_listener<deq_t>qlistener(::ios,&qrecv);

    // listen for messages asynchronously
    BOOST_LOG_TRIVIAL(debug)<<"start sending messages asynchrounously ...";
    string msg{boost::lexical_cast<string>(msgcount++)};
    qsender.timed_async_enq(msg,std::bind(qsender_handler,_1,&qsender),tmo_enq_ms);

    // send messages asynchronously
    BOOST_LOG_TRIVIAL(info)<<"starting waiting for asio message ...";
    qlistener.timed_async_deq(std::bind(qlistener_handler,_1,_2,&qlistener),tmo_deq_ms);

    BOOST_LOG_TRIVIAL(info)<<"starting asio ...";
    ::ios.run();

    // cleanup
    BOOST_LOG_TRIVIAL(info)<<"closing fds ...";
    close(fdwrite);
    close(fdread);

    // wait for child
    BOOST_LOG_TRIVIAL(info)<<"waiting for child (pid: "<<cpid<<") ..."<<endl;
    int waitstat;
    while(waitpid(cpid,&waitstat,0)!=cpid);
  }
  catch(std::exception const&e){
    cerr<<"main: cought exception: "<<e.what()<<endl;
  }
}

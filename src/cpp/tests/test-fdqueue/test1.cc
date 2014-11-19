/*
This program:
	- start child process and setup of queue_sender<> and queue_listener<> connected to child process stdin/stdout
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
#include <vector>
#include <functional>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <boost/log/trivial.hpp>

using namespace std;
using namespace utils;
using namespace std::placeholders;
namespace asio=boost::asio;

// ----- some constants -----
namespace {
size_t msgcount{0};                 // track #of messages sent
constexpr size_t maxmsg{5};        // we'll send this many messages
constexpr size_t tmo_deq_ms{2000};  // timeout for waiting async. for a message
constexpr size_t tmo_enq_ms{1000};  // timeout waiting to be able to sen a message async.
}
// io_service object (we are asio based)
boost::asio::io_service ios;

// serialize an object to be sent (notice: message boundaries are on '\n' characters)
// (normally messages would be serialized and then base64 encoded)
std::function<void(std::ostream&,string const&)>serialiser=[](std::ostream&os,string const&s){
  os<<s;
};
// de-serialize an object we received (notice: message boundaries are on '\n' characters)
// (normally messages would be base64-decoded and then deserialised)
std::function<string(istream&)>deserialiser=[](istream&is){
  string line;
  getline(is,line);
  return line;
};
// queue types used by parent
// (these are queues used by the parent - the child reads/writes directly to stdin/stdout)
using enq_t=asio::fdenq_queue<string,decltype(serialiser)>;
using deq_t=asio::fddeq_queue<string,decltype(deserialiser)>;

// handle a message we received asynchrounosly
void qlistener_handler(boost::system::error_code const&ec,string msg,asio::queue_listener<deq_t>*ql){
  if(ec!=0)BOOST_LOG_TRIVIAL(info)<<"deque() aborted (via asio), cause: "<<ec.message();
  else{
    // kick off waiting for another message asynchrounosly
    BOOST_LOG_TRIVIAL(info)<<"received msg in qlistener_handler (via asio), msg: \""<<msg<<"\", cause: "<<ec;
    ql->timed_async_deq(std::bind(qlistener_handler,_1,_2,ql),tmo_deq_ms);
  }
}
// handler called asynchrounosly when 'it is possible' to send a message through the pipe
void qsender_handler(boost::system::error_code const&ec,asio::queue_sender<enq_t>*qs){
  // print item if error code is OK
  if(ec)BOOST_LOG_TRIVIAL(info)<<"queue sender interupted (via asio): ignoring callback, cause: "<<ec;
  else{
    // check if we are done
    if(msgcount==maxmsg)return;

    // kick off waiting to be able to send another message asynchrounosly
    string msg{boost::lexical_cast<string>(msgcount++)};
    qs->timed_async_enq(msg,std::bind(qsender_handler,_1,qs),tmo_enq_ms);
  }
}
// main for test program
int main(){
  try{
    // spawn '/bin/cat' as child process
    int fdread,fdwrite;
    int cpid=spawnPipeChild("/bin/cat",vector<string>{"cat"},fdread,fdwrite,true);

    // enclose queues in a scope so that fds are closed automatically when leaving scope
    // (if queues are not destroyed, the child process will hang and wait for more input)
    // (we could of course just close the fds ourselves ... by we not be lazy ...)
    {
      // error code from asio calls
      boost::system::error_code ec;

      // create queues (sepcify to close fds on desctruction)
      enq_t qsend{fdwrite,serialiser,true};
      deq_t qrecv{fdread,deserialiser,true};
    
      // setup queue sender/listener
      asio::queue_sender<enq_t>qsender(::ios,&qsend);
      asio::queue_listener<deq_t>qlistener(::ios,&qrecv);

      // kick off listening for messages asynchronously
      BOOST_LOG_TRIVIAL(info)<<"start sending messages asynchrounously ...";
      string msg{boost::lexical_cast<string>(msgcount++)};
      qsender.timed_async_enq(msg,std::bind(qsender_handler,_1,&qsender),tmo_enq_ms);

      // kick off sending messages asynchronously
      BOOST_LOG_TRIVIAL(info)<<"starting waiting for asio message ...";
      qlistener.timed_async_deq(std::bind(qlistener_handler,_1,_2,&qlistener),tmo_deq_ms);

      // run asio
      BOOST_LOG_TRIVIAL(info)<<"starting asio ...";
      ::ios.run();
    }
    // wait for child
    BOOST_LOG_TRIVIAL(info)<<"waiting for child (pid: "<<cpid<<") ..."<<endl;
    int waitstat;
    while(waitpid(cpid,&waitstat,0)!=cpid);
  }
  catch(std::exception const&e){
    cerr<<"main: cought exception: "<<e.what()<<endl;
  }
}

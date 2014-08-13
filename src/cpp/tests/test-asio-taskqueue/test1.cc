#include "asio-extensions/queue_listener.h"
#include "asio-extensions/queue_sender.h"
#include "asio-extensions/simple_queue.h"
#include "xlate-jobs/TaskQueue.h"
#include "xlate-jobs/TranslationTask.h"
#include <boost/asio.hpp> 
#include <boost/log/trivial.hpp>
#include <iostream>
#include <string>
#include <ratio>
#include <memory>
using namespace std;
using namespace std::placeholders;
using namespace xlate;

// asio io_sevice
boost::asio::io_service ios;

// setup request queue stuff
size_t max_outstanding_req{30};
std::shared_ptr<TaskQueue>qreq{make_shared<TaskQueue>(max_outstanding_req)};
boost::asio::simple_queue_sender<std::shared_ptr<TranslationTask>>qreq_sender(::ios,qreq);
boost::asio::simple_queue_listener<std::shared_ptr<TranslationTask>>qreq_listener(::ios,qreq);

// handler for receiving requests
void req_receive(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task){
  if(ec)BOOST_LOG_TRIVIAL(debug)<<"got error: "<<ec;
  else BOOST_LOG_TRIVIAL(debug)<<"got task: "<<*task;
}
// main test program
int main(){
  try{
    // test sending a message to queue (handler function won't do anything)
    std::shared_ptr<TranslationTask>task{make_shared<TranslationTask>(TranslationJobId(),make_lanpair("en","sv"),"Hello world",1)};
    qreq_sender.async_enq(task,[](boost::system::error_code const&ec){});

    // test receiving a message
    qreq_listener.async_deq(req_receive);

    // run io loop
    ::ios.run();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}

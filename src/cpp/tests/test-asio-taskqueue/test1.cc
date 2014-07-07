#include "xlate-jobs/basic_taskq.h"
#include "xlate-jobs/LanguageCode.h"
#include <boost/asio.hpp> 
#include <boost/log/trivial.hpp>
#include <iostream>
#include <thread>
#include <string>
using namespace std;
using namespace xlate;

// send task to a queue
void sendTask(std::shared_ptr<TaskQueue>tq){
  LanguagePair lp{make_lanpair("en","sv")};
  std::shared_ptr<TranslationTask>task{make_shared<TranslationTask>(TranslationJobId(),lp,string("Hello World"),17)};
  BOOST_LOG_TRIVIAL(debug)<<"sending task in separate thread ...";
  tq->enq(task);
}
// main test program
int main(){
  try{
    // io service
    boost::asio::io_service ios; 

    // task queue
    TaskQueueIOService asioq(ios);

    // create queue we want to listen on
    std::shared_ptr<TaskQueue>tq{make_shared<TaskQueue>(10)};

    // send task n separate thread
    std::thread thrSend{[&](){sendTask(tq);}};

    // get tasks from queue synchronously
    std::shared_ptr<TranslationTask>task{asioq.deq(tq)};
    BOOST_LOG_TRIVIAL(debug)<<"asio sync: \""<<*task<<"\"";

    // join sender thread
    thrSend.join();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}

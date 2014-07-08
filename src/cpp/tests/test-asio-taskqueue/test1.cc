#include "xlate-jobs/basic_taskq.h"
#include "xlate-jobs/LanguageCode.h"
#include <boost/asio.hpp> 
#include <boost/bind.hpp> 
#include <boost/lambda/bind.hpp>
#include <boost/log/trivial.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
using namespace std;
using namespace xlate;

// constants
constexpr static size_t maxmsg{1000};

// send task to a queue
void sendTask(std::shared_ptr<TaskQueue>tq){
  LanguagePair lp{make_lanpair("en","sv")};
  std::shared_ptr<TranslationTask>task{make_shared<TranslationTask>(TranslationJobId(),lp,string("Hello World"),17)};
  for(auto i=0;i<maxmsg;++i){
    BOOST_LOG_TRIVIAL(debug)<<"sending task in separate thread ...";
    tq->enq(task);
    this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}
// asio deq handler
class TaskHandler{
public:
  // ctor
  TaskHandler(boost::asio::io_service&ios,std::shared_ptr<TaskQueue>tq):
      ios_(ios),tq_(tq),asioq_(ios_){
    asioq_.async_deq(tq_,boost::bind(&TaskHandler::handleTask,this,boost::asio::placeholders::error,boost::lambda::_2));
  }
  // handle async events
  void handleTask(const boost::system::error_code&ec,std::shared_ptr<TranslationTask>task){
    BOOST_LOG_TRIVIAL(debug)<<"asio: "<<*task;
    asioq_.async_deq(tq_,boost::bind(&TaskHandler::handleTask,this,boost::asio::placeholders::error,boost::lambda::_2));
  }
private:
  boost::asio::io_service&ios_;
  std::shared_ptr<TaskQueue>tq_;
  TaskQueueIOService asioq_;
};
// main test program
int main(){
  try{
    // io service
    boost::asio::io_service ios; 

    // create queue we want to listen on
    std::shared_ptr<TaskQueue>tq{make_shared<TaskQueue>(100)};

    // create task handler and register it with boost asio
    TaskHandler taskHandler(ios,tq);

    // create a deealine timer and register it
    

    // publish tasks
    std::thread thrSend{[&](){sendTask(tq);}};

    // run asio service
    ios.run();

    // join sender thread
    thrSend.join();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}

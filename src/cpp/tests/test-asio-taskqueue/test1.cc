#include "xlate-jobs/basic_taskq.h"
#include "xlate-jobs/LanguageCode.h"
#include <boost/asio.hpp> 
#include <boost/bind.hpp> 
#include <boost/lambda/bind.hpp>
#include <boost/log/trivial.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
using namespace std;
using namespace xlate;

// --- constants
constexpr static size_t maxmsg{1000};
constexpr static size_t tmoSec{3};

// --- send task to a queue
void sendTask(std::shared_ptr<TaskQueue>tq){
  LanguagePair lp{make_lanpair("en","sv")};
  std::shared_ptr<TranslationTask>task{make_shared<TranslationTask>(TranslationJobId(),lp,string("Hello World"),17)};
  for(auto i=0;i<maxmsg;++i){
    BOOST_LOG_TRIVIAL(debug)<<"sending task in separate thread ...";
    tq->enq(task);
    this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}
// --- asio deq handler
void taskHandler(const boost::system::error_code&ec,std::shared_ptr<TranslationTask>task,TaskQueueIOService*asioq,std::shared_ptr<TaskQueue>tq){
    BOOST_LOG_TRIVIAL(debug)<<"asio: "<<*task;
    asioq->async_deq(tq,boost::bind(taskHandler,boost::asio::placeholders::error,boost::lambda::_2,asioq,tq));
}
// timer pop function
void timerPopped(const boost::system::error_code&ec,boost::asio::deadline_timer*tmo){
  BOOST_LOG_TRIVIAL(debug)<<"TMO POPPED";
  tmo->expires_at(tmo->expires_at()+boost::posix_time::seconds(tmoSec));
  tmo->async_wait(boost::bind(timerPopped,boost::asio::placeholders::error,tmo));
}
// main test program
int main(){
  try{
    // io service
    boost::asio::io_service ios; 

    // create queue we want to listen on
    std::shared_ptr<TaskQueue>tq{make_shared<TaskQueue>(100)};

    // create task handler and register it with boost asio
    TaskQueueIOService asioq(ios);
    asioq.async_deq(tq,boost::bind(taskHandler,boost::asio::placeholders::error,boost::lambda::_2,&asioq,tq));

    // create a deealine timer and register it
    boost::asio::deadline_timer tmo(ios,boost::posix_time::seconds(tmoSec));
    tmo.async_wait(boost::bind(timerPopped,boost::asio::placeholders::error,&tmo));

    // publish tasks to queue in separate thread
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

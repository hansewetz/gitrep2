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
constexpr static size_t maxmsg{3};
constexpr static size_t tmoSec{3};

// --- send task to a queue
void sendTask(std::shared_ptr<TaskQueue>tq){
  LanguagePair lp{make_lanpair("en","sv")};
  size_t nsent{0};
  while(true){
    BOOST_LOG_TRIVIAL(debug)<<"sending task in separate thread ...";
    std::shared_ptr<TranslationTask>task{make_shared<TranslationTask>(TranslationJobId(),lp,string("Hello World"),17)};
    tq->enq(task);
    this_thread::sleep_for(std::chrono::milliseconds(1000));
    if(++nsent==maxmsg){
      tq->blockDeq();
// NOTE!
break;
    }
  }
}
// --- asio deq handler
void taskHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task,TaskQueueIOService*asioq){
    if(task.get()==nullptr){
      BOOST_LOG_TRIVIAL(debug)<<"received nullptr task (via asio)";
    }else{
      BOOST_LOG_TRIVIAL(debug)<<"received task (via asio): "<<*task;
      asioq->async_deq(boost::bind(taskHandler,boost::asio::placeholders::error,boost::lambda::_2,asioq));
    }
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
    TaskQueueIOService asioq1(ios,tq);
// NOTE!    TaskQueueIOService asioq2(tq,ios);
    asioq1.async_deq(boost::bind(taskHandler,boost::asio::placeholders::error,boost::lambda::_2,&asioq1));
// NOTE!    asioq2.async_deq(tq,boost::bind(taskHandler,boost::asio::placeholders::error,boost::lambda::_2,&asioq2));

    // create a deadline timer and register it
/* NOTE!
    boost::asio::deadline_timer tmo(ios,boost::posix_time::seconds(tmoSec));
    tmo.async_wait(boost::bind(timerPopped,boost::asio::placeholders::error,&tmo));
*/
    // publish tasks to queue in separate thread
    std::thread thrSend{[&](){sendTask(tq);}};

    // run asio service
    ios.run();
// NOTE!
cerr<<"back from run()"<<endl;

    // join sender thread
    thrSend.join();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}

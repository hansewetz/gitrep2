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
constexpr static size_t maxmsg{5};
constexpr static size_t tmoSec{3};

// --- send task to a queue
void sendTask(std::shared_ptr<TaskQueue>tq){
  size_t nsent{0};
  LanguagePair lp{make_lanpair("en","sv")};
  while(true){
    BOOST_LOG_TRIVIAL(debug)<<"sending task in separate thread ...";
    std::shared_ptr<TranslationTask>task{make_shared<TranslationTask>(TranslationJobId(),lp,string("Hello World"),17)};
    tq->enq(task);
    this_thread::sleep_for(std::chrono::milliseconds(250));
    if(nsent++==maxmsg)break;
  }
}
// --- asio deq handler
atomic_size_t nreceived{0};
void taskHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task,TaskQueueIOService*asioq,std::shared_ptr<TaskQueue>tq){
  if(nreceived++>=maxmsg){
    BOOST_LOG_TRIVIAL(debug)<<"have received eneough messages";
// NOTE!
tq->blockDeq();
  }else{
    BOOST_LOG_TRIVIAL(debug)<<"received task (via asio): "<<(task?"non-null":"null");
    asioq->async_deq(tq,boost::bind(taskHandler,boost::asio::placeholders::error,boost::lambda::_2,asioq,tq));
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
    // create queue we want to listen on
    std::shared_ptr<TaskQueue>tq{make_shared<TaskQueue>(100)};

    // publish tasks to queue in separate thread
    std::thread thrSend{[&](){sendTask(tq);}};
    {
    // io service
    boost::asio::io_service ios; 

    // create task handler and register it with boost asio
    TaskQueueIOService asioq1(ios);
    TaskQueueIOService asioq2(ios);
    asioq1.async_deq(tq,boost::bind(taskHandler,boost::asio::placeholders::error,boost::lambda::_2,&asioq1,tq));
    asioq2.async_deq(tq,boost::bind(taskHandler,boost::asio::placeholders::error,boost::lambda::_2,&asioq2,tq));

    // create a deadline timer and register it
/* NOTE!
    boost::asio::deadline_timer tmo(ios,boost::posix_time::seconds(tmoSec));
    tmo.async_wait(boost::bind(timerPopped,boost::asio::placeholders::error,&tmo));
*/

    // run asio service
    ios.run();
cerr<<"back from run()"<<endl<<std::flush;
  }
cerr<<"outside block"<<endl<<std::flush;

  // join sender thread
cerr<<"joining sender thread"<<endl;
  thrSend.join();
cerr<<"done joining sender thread"<<endl;
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}

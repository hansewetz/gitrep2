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
constexpr static size_t maxtmo{3};
constexpr static size_t tmoSec{3};
constexpr static size_t tmoSeleepBetweenSendMs{250};

// --- send task to a queue
void sendTask(std::shared_ptr<TaskQueue>tq){
  size_t nsent{0};
  LanguagePair lp{make_lanpair("en","sv")};
  while(true){
    BOOST_LOG_TRIVIAL(debug)<<"sending task in separate thread ...";
    std::shared_ptr<TranslationTask>task{make_shared<TranslationTask>(TranslationJobId(),lp,string("Hello World"),17)};
    tq->enq(task);
    this_thread::sleep_for(std::chrono::milliseconds(tmoSeleepBetweenSendMs));
    if(++nsent==maxmsg)break;
  }
}
// --- asio deq handler
size_t nreceived1{0};
void taskHandler1(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task,TaskQueueIOService*asioq,std::shared_ptr<TaskQueue>tq){
  BOOST_LOG_TRIVIAL(debug)<<"received task in Q1 (via asio): "<<(task?"non-null":"null")<<", ec: "<<ec;
  if(++nreceived1>=maxmsg){
    BOOST_LOG_TRIVIAL(debug)<<"have received eneough messages in Q1";
    tq->blockDeq();
  }else{
    asioq->async_deq(tq,boost::bind(taskHandler1,boost::asio::placeholders::error,boost::lambda::_2,asioq,tq));
  }
}
size_t nreceived2{0};
void taskHandler2(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task,TaskQueueIOService*asioq,std::shared_ptr<TaskQueue>tq){
  BOOST_LOG_TRIVIAL(debug)<<"received task in Q2 (via asio): "<<(task?"non-null":"null")<<", ec: "<<ec;
  if(++nreceived2>=maxmsg){
    BOOST_LOG_TRIVIAL(debug)<<"have received eneough messages in Q1";
    tq->blockDeq();
  }else{
    asioq->async_deq(tq,boost::bind(taskHandler2,boost::asio::placeholders::error,boost::lambda::_2,asioq,tq));
  }
}
// timer pop function
size_t ntmos{0};
void timerPopped(const boost::system::error_code&ec,boost::asio::deadline_timer*tmo){
  BOOST_LOG_TRIVIAL(debug)<<"tmo popped";
  if(++ntmos<maxtmo){
    tmo->expires_at(tmo->expires_at()+boost::posix_time::seconds(tmoSec));
    tmo->async_wait(boost::bind(timerPopped,boost::asio::placeholders::error,tmo));
  }
}
// main test program
int main(){
  try{
    // create queue we want to listen on
    std::shared_ptr<TaskQueue>tq1{make_shared<TaskQueue>(100)};
    std::shared_ptr<TaskQueue>tq2{make_shared<TaskQueue>(100)};

    // publish tasks to queues in separate threads
    std::thread thrSend1{[&](){sendTask(tq1);}};
    std::thread thrSend2{[&](){sendTask(tq2);}};
    {
      // io service
      boost::asio::io_service ios; 

      // create task handler and register it with boost asio
      TaskQueueIOService asioq1(ios);
      asioq1.async_deq(tq1,boost::bind(taskHandler1,boost::asio::placeholders::error,boost::lambda::_2,&asioq1,tq1));
      TaskQueueIOService asioq2(ios);
      asioq2.async_deq(tq2,boost::bind(taskHandler2,boost::asio::placeholders::error,boost::lambda::_2,&asioq1,tq2));

      // create a deadline timer and register it
      boost::asio::deadline_timer tmo(ios,boost::posix_time::seconds(tmoSec));
      tmo.async_wait(boost::bind(timerPopped,boost::asio::placeholders::error,&tmo));

      // run asio service
      ios.run();
    }
    // join sender threads
    thrSend1.join();
    thrSend2.join();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}

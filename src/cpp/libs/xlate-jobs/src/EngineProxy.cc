/*
NOTE!
-----
we are simulating translation by setting up a timer after receiving a task
when the timer pops we send a 'translated' task to output queue

what we should do is this: (we'll go between states: [WAITING-FOR-TASK and WAITING-FOR-TRANSLATION] with events (GOT-TASK, GOT-TMO, GOT-TRANSLATION)
	- wait for a task asyncrounosly
	- when receiving task setup a timer, setup a listener on engine reception and send task text to engine
	- if time pops then the engine timed out - should kill engine and restrt it
	- if we receive a response form engine we cancel the timer and send the task on output queue
*/
#include "xlate-jobs/EngineProxy.h"
#include "xlate-jobs/TranslationTask.h"
#include <boost/log/trivial.hpp>
#include <chrono>
#include <random>
#include <boost/asio.hpp>
using namespace std;
using namespace std::placeholders;
namespace xlate{

// ctor
EngineProxy::EngineProxy(boost::asio::io_service&ios,std::shared_ptr<TaskQueue>qin,std::shared_ptr<TaskQueue>qout):
    ios_(ios),
    qtaskListener_(make_shared<TaskQueueListener>(ios_,qin.get())),
    qtaskSender_(make_shared<TaskQueueSender>(ios_,qout.get())),
    tmo_(ios_,boost::posix_time::milliseconds(1)){
}
// wait for new task asynchronously
void EngineProxy::run(){
  waitForNewTask();
}
// get engine id
EngineProxyId EngineProxy::id()const{
  return id_;
}
// wait for new task
void EngineProxy::waitForNewTask(){
  BOOST_LOG_TRIVIAL(debug)<<"EngineProxy::waitForNewTask - wating for new task to translate";
  qtaskListener_->async_deq(std::bind(&EngineProxy::newTaskHandler,this,_1,_2));
}
// handle a new task
void EngineProxy::newTaskHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task){
  BOOST_LOG_TRIVIAL(debug)<<"EngineProxy::newTaskHandler - got task event: "<<*task;

  // generate a random timeout time for translation
  // (should be a fixed timeout when calling a real engine)
  std::random_device rd;
  std::default_random_engine e1(rd());
  std::uniform_int_distribution<int>uniform_dist(500, 5000);
  int ms_tmo{uniform_dist(e1)};

  // start translation
  // cout<<"engine: "<<id_<<", job: "<<task->jobid()<<endl;
  // (simulate translation by setting up a timer and generate a 'translation' when timer pops)
  tmo_.expires_from_now(boost::posix_time::milliseconds(ms_tmo));
  tmo_.async_wait(std::bind(&EngineProxy::tmoHandler,this,_1,task));
}
// handle translation timeout
// (here we simulate translation through the timeout)
void EngineProxy::tmoHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task){
  BOOST_LOG_TRIVIAL(debug)<<"EngineProxy::tmoHandler - got timer event: "<<*task;
  // 'translate' task and send it on output queue
  task->setTargetSeg(string("TRANSLATED: ")+task->srcSeg());

  // send task synchronously
  boost::system::error_code ec1;
  qtaskSender_->sync_enq(task,ec1);

// NOTE! Should check error code here

  // start waiting for a new task
  waitForNewTask();
}
}

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "xlate-jobs/EngineProxy.h"
#include "xlate-jobs/EngineEnv.h"
#include "xlate-jobs/TranslationTask.h"
#include "utils/sysUtils.h"
#include <boost/log/trivial.hpp>
#include <chrono>
#include <random>
#include <sys/types.h>
#include <sys/wait.h>
#include <boost/asio.hpp>

using namespace std;
using namespace placeholders;
namespace asio=boost::asio;

namespace xlate{

// serialise/deserialise segments to/from engines
namespace{
  // serialize an object to be sent (notice: message boundaries are on '\n' characters)
  void serialiser(std::ostream&os,string const&s){
    os<<s;
  }
  // de-serialize an object we received (notice: message boundaries are on '\n' characters)
  string deserialiser(istream&is){
    string line;
    getline(is,line);
    return line;
  }
}

// ctor
EngineProxy::EngineProxy(asio::io_service&ios,shared_ptr<TaskQueue>qin,shared_ptr<TaskQueue>qout,shared_ptr<EngineEnv>engineenv):
    ios_(ios),
    qtaskListener_(make_shared<TaskQueueListener>(ios_,qin.get())),
    qtaskSender_(make_shared<TaskQueueSender>(ios_,qout.get())),
    state_{state_t(EngineProxy::state_t::NOT_RUNNING)},
    cpid_(-1),engineenv_(engineenv),ntmos_(0),realTmoMs_(engineenv->startTmoMs()){
}
// dtor
EngineProxy::~EngineProxy(){
  stopEngine();
}
// wait for new task asynchronously
void EngineProxy::run(){
  // check if we can start engine
  if(state_!=EngineProxy::state_t::NOT_RUNNING)return;

  BOOST_LOG_TRIVIAL(debug)<<"starting new engine with id: "<<id_<<" ...";

  // we are not running so reset some variables
  realTmoMs_=engineenv_->startTmoMs();
  ntmos_=0;
  cpid_=-1;

  // start engine (we should never be her unless engine is not running)
  int fdToEngine;
  int fdFromEngine;
  cpid_=utils::spawnPipeChild(engineenv_->progpath().string(),vector<string>{engineenv_->ename()},fdFromEngine,fdToEngine,true,engineenv_->exedir().string());

  // create queues to/from engine
  qToEngine_=make_shared<qToEngine_t>(fdToEngine,serialiser,true);
  qFromEngine_=make_shared<qFromEngine_t>(fdFromEngine,deserialiser,true);

  // create sender/listener to/from engines
  // (setup to cloe fds on destruction)
  qsenderToEngine_=make_shared<asio::queue_sender<qToEngine_t>>(ios_,qToEngine_.get());
  qListenerFromEngine_=make_shared<asio::queue_listener<qFromEngine_t>>(ios_,qFromEngine_.get());

  // wait for a task to translate
  waitForNewTask();
  BOOST_LOG_TRIVIAL(debug)<<"engine with id: "<<id_<<" started";
}
// stop engine
void EngineProxy::stop(){
  // nothing to do if we are not running
  if(state_==EngineProxy::state_t::NOT_RUNNING)return;

  // wait for engine to stop
  stopEngine();
}
// get engine id
EngineProxyId EngineProxy::id()const{
  return id_;
}
// wait for new task
void EngineProxy::waitForNewTask(){
  BOOST_LOG_TRIVIAL(debug)<<"EngineProxy::waitForNewTask - wating for new task to translate";
  qtaskListener_->async_deq(bind(&EngineProxy::newTaskHandler,this,_1,_2));
  state_=EngineProxy::state_t::WAITING4TASK;
}
// handle a new task to be translated
void EngineProxy::newTaskHandler(boost::system::error_code const&ec,shared_ptr<TranslationTask>task){
  // if operation was aborted we stop engine
  if(ec==boost::asio::error::operation_aborted){
    BOOST_LOG_TRIVIAL(debug)<<"EngineProxy::engineListenerHandler - sending of segment to engine aborted";
    stop();
    return;
  }else
  if(ec!= boost::system::error_code()){
    BOOST_LOG_TRIVIAL(debug)<<"EngineProxy::engineListenerHandler - sending of segment to engine failed, ec: "<<ec.message();
    stop();
    return;
  }
  BOOST_LOG_TRIVIAL(debug)<<"EngineProxy::newTaskHandler - got task event: "<<*task;

  // start translation synchrounosly (send segment to qtaskSender_)
  // (notice: we send segment synchrounously)
  boost::system::error_code ec1;
  qsenderToEngine_->sync_enq(task->srcSeg(),ec1);

  // we are now translating
  state_=EngineProxy::state_t::TRANSLATING;

  // start waiting for translated segment back from engine
  qListenerFromEngine_->timed_async_deq(std::bind(&EngineProxy::engineListenerHandler,this,_1,_2,task),realTmoMs_);
}
// handler for segments arriving from engine
void EngineProxy::engineListenerHandler(boost::system::error_code const&ec,string const&msg,shared_ptr<TranslationTask>task){
  // check for aborted - in this case we stop engine
  if(ec==boost::asio::error::operation_aborted){
    BOOST_LOG_TRIVIAL(debug)<<"EngineProxy::engineListenerHandler - sending of segment to engine aborted";
    stop();
    return;
  }
  if(ec==boost::asio::error::timed_out){
    BOOST_LOG_TRIVIAL(debug)<<"EngineProxy::engineListenerHandler - timeout";
    task->setTimeout(id_);
    ++ntmos_;

    // send repsonse and wait for new task
    boost::system::error_code ec1;
    qtaskSender_->sync_enq(task,ec1);
    waitForNewTask();
    return;
  }
  if(ec!=boost::system::error_code()){
    BOOST_LOG_TRIVIAL(debug)<<"EngineProxy::engineListenerHandler - sending of segment to engine failed, ec: "<<ec.message();
    stop();
    return;
  }
  // we got a segment back so we can set tmo to the real segment tmo
  realTmoMs_=engineenv_->tmoMs();

  // set translated segment unless we have outstanding timeoued out segments
  BOOST_LOG_TRIVIAL(debug)<<"EngineProxy::engineListenerHandler - got translated task: "<<*task;
  if(ntmos_==0){
    // send repsonse and wait for new task
    task->setTargetSeg(msg,id_);
    boost::system::error_code ec1;
    qtaskSender_->sync_enq(task,ec1);
    waitForNewTask();
  }
  else{
    // listen for new segment - there is nothing to send back since we have already sent a TIMEOUT
    BOOST_LOG_TRIVIAL(debug)<<"EngineProxy::engineListenerHandler - got timedout translated task - trashing result ...";
    qListenerFromEngine_->timed_async_deq(std::bind(&EngineProxy::engineListenerHandler,this,_1,_2,task),realTmoMs_);
     --ntmos_;
  }
}
// helper to stopo an engine
void EngineProxy::stopEngine(){
  // if engine is running we must wait for it
  if(state_!=EngineProxy::state_t::NOT_RUNNING){
    BOOST_LOG_TRIVIAL(debug)<<"stopping engine with id: "<<id_<<", pid: "<<cpid_<<" ...";

    // set state to not running
    state_=EngineProxy::state_t::NOT_RUNNING;

    // stop engine by closing pipe
    ::close(qToEngine_->getfd());

    // wait for engine to stop
    int waitstat;
    BOOST_LOG_TRIVIAL(debug)<<"waiting for child (pid: "<<cpid_<<") ...";
    if(cpid_!=-1){
      // For some reason we cannot wait for a specific pid without killing it first.
      // if I just close the pipe, then waitpid will hang

      // kill process to be sure
      kill(cpid_,SIGHUP);

      // wait for it - make sure it won't become a zombie
      while(waitpid(cpid_,&waitstat,0)!=-1);
      BOOST_LOG_TRIVIAL(debug)<<"pid: "<<cpid_<<" stopped";
      cpid_=-1;
    }
  }
}
}

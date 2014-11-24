// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "xlate-jobs/EngineProxy.h"
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
EngineProxy::EngineProxy(asio::io_service&ios,shared_ptr<TaskQueue>qin,shared_ptr<TaskQueue>qout):
    ios_(ios),
    qtaskListener_(make_shared<TaskQueueListener>(ios_,qin.get())),
    qtaskSender_(make_shared<TaskQueueSender>(ios_,qout.get())),
    state_{state_t(EngineProxy::state_t::NOT_RUNNING)},
    cpid_(-1){
}
// dtor
EngineProxy::~EngineProxy(){
  // if engine is running we must wait for it
  if(state_!=EngineProxy::state_t::NOT_RUNNING){
    int waitstat;
    BOOST_LOG_TRIVIAL(debug)<<"waiting for child (pid: "<<cpid_<<") ..."<<endl;
    while(waitpid(cpid_,&waitstat,0)!=cpid_);
  }
}
// wait for new task asynchronously
void EngineProxy::run(){
  BOOST_LOG_TRIVIAL(debug)<<"starting new engine with id: "<<id_<<" ...";

  // start engine (we should never be her unless engine is not running)
  int fdToEngine;
  int fdFromEngine;
  cpid_=utils::spawnPipeChild(PROGPATH,vector<string>{PROGNAME},fdFromEngine,fdToEngine,true,PROGDIR);

  // create queues to/from engine
  qToEngine_=make_shared<qToEngine_t>(fdToEngine,serialiser,true);
  qFromEngine_=make_shared<qFromEngine_t>(fdFromEngine,deserialiser,true);

  // create sender/listener to/from engines
  qsenderToEngine_=make_shared<asio::queue_sender<qToEngine_t>>(ios_,qToEngine_.get());
  qListenerFromEngine_=make_shared<asio::queue_listener<qFromEngine_t>>(ios_,qFromEngine_.get());

  // wait for a task to translate
  waitForNewTask();
  BOOST_LOG_TRIVIAL(debug)<<"engine with id: "<<id_<<" started";
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
// handle a new task
void EngineProxy::newTaskHandler(boost::system::error_code const&ec,shared_ptr<TranslationTask>task){
  BOOST_LOG_TRIVIAL(debug)<<"EngineProxy::newTaskHandler - got task event: "<<*task;

  // start translation synchrounosly (send segment to qtaskSender_)
  // (notice: we send segment synchrounously)
  boost::system::error_code ec1;
  qsenderToEngine_->sync_enq(task->srcSeg(),ec1);

  // Check error code
  // NOTE! Not yet done

  // we are now translating
  state_=EngineProxy::state_t::TRANSLATING;

  // start waiting for translated segment back from engine
  qListenerFromEngine_->timed_async_deq(std::bind(&EngineProxy::engineListenerHandler,this,_1,_2,task),xlateTmoMs_);
}
// handler for segments arriving from engine
void EngineProxy::engineListenerHandler(boost::system::error_code const&ec,string const&msg,shared_ptr<TranslationTask>task){
  // check for timeout
  if(ec==boost::asio::error::timed_out){
// NOTE! Do something better
    task->setTargetSeg("<TIMEOUT>");
  }else{
    // check error code 
    // NOTE! Not yet done

    // set translated segment
    BOOST_LOG_TRIVIAL(debug)<<"EngineProxy::engineListenerHandler - got translated task: "<<*task;
    task->setTargetSeg(msg);
  }
  // send translated task
  boost::system::error_code ec1;
  qtaskSender_->sync_enq(task,ec1);

  // check error code
  // NOTE! Not yet done

  // start waiting for a new task
  waitForNewTask();
}
}

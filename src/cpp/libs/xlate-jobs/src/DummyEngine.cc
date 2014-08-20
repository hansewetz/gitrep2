#include "xlate-jobs/DummyEngine.h"
#include "xlate-jobs/TranslationTask.h"
#include <boost/log/trivial.hpp>
#include <functional>
using namespace std;
using namespace std::placeholders;
namespace xlate{

// ctor
DummyEngine::DummyEngine(std::shared_ptr<TaskQueue>qin,std::shared_ptr<TaskQueue>qout):
    qin_(qin),qout_(qout){
}
// enable wait for new task
void DummyEngine::run(){
  while(true){
    BOOST_LOG_TRIVIAL(debug)<<"DummyEngine::waitForNewTask - enabling receiving task";
    pair<bool,std::shared_ptr<TranslationTask>>p{qin_->deq()};
    if(p.first==false){
      BOOST_LOG_TRIVIAL(debug)<<"DummyEngine::waitForNewTask (id:"<<id_<<") - received interuption: terminating";
      break;
    }
    // get task and process it
    std::shared_ptr<TranslationTask>task{p.second};
    task->setTargetSeg(task->srcSeg()+" [engine id: "+id_.asString()+"]");    // NOTE! Should translate here

    // send task to output queue
    qout_->enq(task);
  }
}
// get engine id
DummyEngineId DummyEngine::id()const{
  return id_;
}
}

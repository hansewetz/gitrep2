#include "xlate-jobs/EngineAdaptor.h"
#include "xlate-jobs/TranslationTask.h"
#include <boost/log/trivial.hpp>
#include <functional>

#include <chrono>
#include <random>
using namespace std;
using namespace std::placeholders;
namespace xlate{

// ctor
EngineAdaptor::EngineAdaptor(std::shared_ptr<TaskQueue>qin,std::shared_ptr<TaskQueue>qout):
    qin_(qin),qout_(qout){
}
// wait for new task synchronously
void EngineAdaptor::run(){
  while(true){
    BOOST_LOG_TRIVIAL(debug)<<"EngineAdaptor::waitForNewTask - sync waiting for new task";
    pair<bool,std::shared_ptr<TranslationTask>>p{qin_->deq()};
    if(p.first==false){
      BOOST_LOG_TRIVIAL(debug)<<"EngineAdaptor::waitForNewTask (id:"<<id_<<") - received interuption: terminating";
      break;
    }
    // translate task and sent it on output queue
    translate(p.second);
    qout_->enq(p.second);
  }
}
// get engine id
EngineAdaptorId EngineAdaptor::id()const{
  return id_;
}
// transate a task
void EngineAdaptor::translate(std::shared_ptr<TranslationTask>task)const{
  // generate a random sleep time
  std::random_device rd;
  std::default_random_engine e1(rd());
  std::uniform_int_distribution<int>uniform_dist(500, 5000);
  int msSleep{uniform_dist(e1)};

  // sleep for a random time
  std::this_thread::sleep_for(std::chrono::milliseconds(msSleep));

  // set 'translated segment'
  task->setTargetSeg(task->srcSeg()+" [engine id: "+id_.asString()+"]");    // NOTE! Should translate here
}
}

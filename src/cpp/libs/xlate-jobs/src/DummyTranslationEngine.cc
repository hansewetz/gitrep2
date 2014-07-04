#include "xlate-jobs/DummyTranslationEngine.h"
#include "xlate-jobs/TaskQueue.h"
#include "xlate-jobs/TranslationTask.h"
#include <boost/log/trivial.hpp>
#include <iostream>
namespace xlate{

// ctor
DummyTranslationEngine::DummyTranslationEngine(shared_ptr<TaskQueue>inq,shared_ptr<TaskQueue>outq):
    inq_(inq),outq_(outq){
}
// run task collector (moves tasks from task queue into job repository)
void DummyTranslationEngine::operator()(){
  shared_ptr<TranslationTask>task;
  while(task=inq_->deq(true)){
    // prit debug message
    BOOST_LOG_TRIVIAL(debug)<<"translating segment no: "<<task->segno()<<", in job: "<<task->jobid()<<", task: "<<task->id()<<
                              ", from: "<<task->lanpair().first<<" to "<<task->lanpair().second<<", text: "<<task->srcSeg();

    // set transation in task and send task back
    task->setTargetSeg(string("TRANSLATED: ")+task->srcSeg());
    outq_->enq(task);
  }
  // send null task to stop receiver
  outq_->enq(task);
}
// debug print operator
ostream&operator<<(ostream&os,DummyTranslationEngine const&dt){
  return os<<"#inq: "<<dt.inq_->size()<<", #outq: "<<dt.outq_->size();
}
}

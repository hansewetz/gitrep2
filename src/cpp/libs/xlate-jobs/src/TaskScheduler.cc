#include "xlate-jobs/TaskScheduler.h"
#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TranslationTask.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TaskQueue.h"
#include "utils/utility.h"
#include <iostream>
using namespace std;
namespace xlate{
// ctor
TaskScheduler::TaskScheduler(shared_ptr<TranslationJobRepository>jobrep,shared_ptr<TaskQueue>taskq):
  jobrep_(jobrep),taskq_(taskq),done_(false){
}
// run task collector (moves tasks from task queue into job repository)
void TaskScheduler::operator()(){


// NOTE! Must have condition variable so we can wake up when don_ flag has changed ...
//	or some other mechanism to tell this function to exit
//	possible have 'terminate()' inform repository to cancel any waiting threads ... maybe the best way, using callbacks ... ???

  
// loop over jobs in job repository
  while(true){
    // get next job to process
    shared_ptr<TranslationJob>job{jobrep_->startJob()};
    
    // feed all tasks to consumer
    while(true){
      // if done, terminate, else get next task
      lock_guard<mutex>lock(mtx_);
      if(done_)break;
      taskq_->enq(job->getNextTask());
    }
  }
}
// terminate task scheduling permanently (typically called from a different thread)
void TaskScheduler::terminate(){
  // send null task to stop translation and notify any waiting threads
  lock_guard<mutex>lock(mtx_);
  taskq_->enq(shared_ptr<TranslationTask>(nullptr));
  done_=true;
}
// debug print operator
ostream&operator<<(ostream&os,TaskScheduler const&tc){
  return os<<"#waiting tasks: "<<tc.taskq_->size();
}
}

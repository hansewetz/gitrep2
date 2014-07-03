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
  jobrep_(jobrep),taskq_(taskq){
}
// run task collector (moves tasks from task queue into job repository)
void TaskScheduler::operator()(){
  // loop over jobs in job repository
  while(true){
    // get next job to process
    shared_ptr<TranslationJob>job{jobrep_->startJob()};
    
    // feed all tasks to consumer
    shared_ptr<TranslationTask>task;
    while(task=job->getNextTask())taskq_->enq(task);

    // NOTE! Break after one job
    break;
  }
// NOTE!
  // send null task to stop translation
  taskq_->enq(shared_ptr<TranslationTask>(nullptr));
}
// debug print operator
ostream&operator<<(ostream&os,TaskScheduler const&tc){
  return os<<"#waiting tasks: "<<tc.taskq_->size();;
}
}

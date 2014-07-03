#include "xlate-jobs/TaskCollector.h"
#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TranslationTask.h"
#include "xlate-jobs/TaskQueue.h"
#include "utils/utility.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctor
TaskCollector::TaskCollector(shared_ptr<TranslationJobRepository>jobrep,shared_ptr<TaskQueue>taskq):
    jobrep_(jobrep),taskq_(taskq){
}
// run task collector (moves tasks from task queue into job repository)
void TaskCollector::operator()(){
  // loop forever
  shared_ptr<TranslationTask>task;
  while((task=taskq_->deq(true))&&task){
    jobrep_->addTask(task);
  }
}
// debug print function
ostream&operator<<(ostream&os,TaskCollector const&tc){
  return os<<"#waiting tasks: "<<tc.taskq_->size();;
}
}

#include "xlate-jobs/TaskCollector.h"
#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TranslationTask.h"
#include "xlate-jobs/TaskQueue.h"
#include <boost/log/trivial.hpp>
#include <iostream>
using namespace std;
namespace xlate{

// ctor
TaskCollector::TaskCollector(shared_ptr<TranslationJobRepository>jobrep,shared_ptr<TaskQueue>taskq):
    jobrep_(jobrep),taskq_(taskq){
}
// run task collector (moves tasks from task queue into job repository)
void TaskCollector::run(){
  // loop forever
  while(true){
    // get task from queue in blocking mode and move it into repository
    shared_ptr<TranslationTask>task{taskq_->deq(true)};
    if(!jobrep_->addTask(task)){
      // job must have been removed
      BOOST_LOG_TRIVIAL(warning)<<"failed attempt to add task: "<<*task<<" to job repository - likely cause: job has been removed from repository";
    }
  }
}
// debug print function
ostream&operator<<(ostream&os,TaskCollector const&tc){
  return os<<"#waiting tasks: "<<tc.taskq_->size();;
}
}

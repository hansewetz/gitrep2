#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TranslationTask.h"
#include "xlate-jobs/LanguageCode.h"
#include "xlate-jobs/TaskQueue.h"
#include <vector>
#include <iostream>
#include <string>
#include <utility>
#include <thread>
using namespace std;
using namespace xlate;

// consume tasks
void translateTasks(shared_ptr<TaskQueue>qin,shared_ptr<TaskQueue>qout){
  shared_ptr<TranslationTask>task;
  while(task=qin->deq(true))qout->enq(task);
}
// produce tasks
void scheduleTasks(shared_ptr<TranslationJob>job,shared_ptr<TaskQueue>qout){
  // feed all tasks to consumer
  shared_ptr<TranslationTask>task;
  while(task=job->getNextTask())qout->enq(task);
}
// read translated task
void collect(shared_ptr<TranslationJob>job,shared_ptr<TaskQueue>qin){
  // read translated tasks
  shared_ptr<TranslationTask>task;
  while(task=qin->deq(true)){
    job->addTranslatedTask(task);
  }
}
// main test program
int main(){
  // create a request
  LanguageCode src{"en"};
  LanguageCode target{"sv"};
  TranslateRequest req{src,target,{"Hello world","second phrase","third phrase"}};

  // create a job with tasks
  shared_ptr<TranslationJob>job{make_shared<TranslationJob>(req)};

  // print job before translation
  cerr<<"JOB: "<<*job<<endl;

  // create queues
  shared_ptr<TaskQueue>qout{make_shared<TaskQueue>(10)};
  shared_ptr<TaskQueue>qin{make_shared<TaskQueue>(10)};

  // create threads
  thread thr_translate{translateTasks,qout,qin};
  thread thr_schedule(scheduleTasks,job,qout);
  thread thr_collect(collect,job,qin);

  // join threads
  thr_collect.join();
  thr_schedule.join();
  thr_translate.join();

  // print job after translation
  cerr<<"JOB: "<<*job<<endl;
}

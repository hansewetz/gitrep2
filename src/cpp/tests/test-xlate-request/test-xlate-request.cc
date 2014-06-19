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

// translate tasks
void translateTasks(shared_ptr<TaskQueue>qin,shared_ptr<TaskQueue>qout){
  shared_ptr<TranslationTask>task;
  while(task=qout->deq(true)){
    // translate segment and add it to task
    // ...
    task->setTargetSeg(string("TRANSLATED: ")+task->srcSeg());

    // send translated task back to whoever wants to process it
    qin->enq(task);
  }
  // send null task to stop receiver
  qin->enq(task);
}
// read tasks from the job and send them fr translation
void scheduleTasks(shared_ptr<TranslationJob>job,shared_ptr<TaskQueue>qout){
  // feed all tasks to consumer
  shared_ptr<TranslationTask>task;
  while(task=job->getNextTask())qout->enq(task);

 // send null task to stop translation
  qout->enq(shared_ptr<TranslationTask>(nullptr));
}
// read translated task and add them back to the job
void collect(shared_ptr<TranslationJob>job,shared_ptr<TaskQueue>qin){
  // read translated tasks (will end when we receive a null task)
  shared_ptr<TranslationTask>task;
  while((task=qin->deq(true))&&task){
    job->addTranslatedTask(task);
  }
}
// main test program
int main(){
  // create a request
  LanguagePair lp{make_lanpair("en","sv")};
  TranslateRequest req{lp,{"Hello world","second phrase","third phrase"}};

  // create a job from request
  shared_ptr<TranslationJob>job{make_shared<TranslationJob>(req)};
  cerr<<"JOB: "<<*job<<endl;

  // create queues
  shared_ptr<TaskQueue>qout{make_shared<TaskQueue>(10)};
  shared_ptr<TaskQueue>qin{make_shared<TaskQueue>(10)};

  // create threads (producer, translator and consumer of translated segments)
  thread thr_translate{translateTasks,qin,qout};
  thread thr_schedule{scheduleTasks,job,qout};
  thread thr_collect{collect,job,qin};

  // join threads
  thr_collect.join();
  thr_schedule.join();
  thr_translate.join();

  // print job after translation
  cerr<<"JOB: "<<*job<<endl;
}

#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJobRepository.h"
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
void translateTasks(shared_ptr<TaskQueue>q2process,shared_ptr<TaskQueue>qprocessed){
  shared_ptr<TranslationTask>task;
  while(task=q2process->deq(true)){
    // translate segment and add it to task
    // ...
    cout<<"translating segment no: "<<task->segno()<<", from : "<<task->lanpair().first<<" to "<<task->lanpair().second<<", text: "<<task->srcSeg()<<endl;;
    task->setTargetSeg(string("TRANSLATED: ")+task->srcSeg());

    // send translated task back to whoever wants to process it
    qprocessed->enq(task);
  }
  // send null task to stop receiver
  qprocessed->enq(task);
}
// read job from job repository and schedule tasks
void scheduleTasks(shared_ptr<TranslationJobRepository>jobRepos,shared_ptr<TaskQueue>q2process){
  // loop over jobs in job repository
  while(true){
    // get next job to process
    shared_ptr<TranslationJob>job{jobRepos->startJob()};
    
    // feed all tasks to consumer
    shared_ptr<TranslationTask>task;
    while(task=job->getNextTask())q2process->enq(task);

    // NOTE! Break after one job
    break;
  }
  // send null task to stop translation
  q2process->enq(shared_ptr<TranslationTask>(nullptr));
}
// read translated task and add them back to the job
void collect(shared_ptr<TranslationJobRepository>jobRepos,shared_ptr<TaskQueue>qprocessed){
  // read translated tasks and add them to the job where they came from (will end when we receive a null task)
  shared_ptr<TranslationTask>task;
  while((task=qprocessed->deq(true))&&task){
    // lookup job from job repository and add task to job
    shared_ptr<TranslationJob>job{jobRepos->getStartedJob(task->jobid())};
    job->addTranslatedTask(task);
  }
}
// ------- helper functions
// create a request
shared_ptr<TranslateRequest>createRequest(LanguagePair const&lp,initializer_list<string>segs){
  return make_shared<TranslateRequest>(lp,segs);
}
// create a job repository
shared_ptr<TranslationJobRepository>createJobRepos(LanguagePair const&lp){
  return make_shared<TranslationJobRepository>(lp);
}
// create a job
shared_ptr<TranslationJob>createJob(shared_ptr<TranslateRequest>req){
  return make_shared<TranslationJob>(req);
}

// main test program
int main(){
  // create language pair for this test
  LanguagePair lp{make_lanpair("en","sv")};

  // create request and a job from request
  shared_ptr<TranslateRequest>req{createRequest(lp,{"Hello world","second phrase","third phrase"})};
  shared_ptr<TranslationJob>job{createJob(req)};
  cerr<<"JOB: "<<*job<<endl;

  // create a job repository and add a job
  shared_ptr<TranslationJobRepository>jobRepos{createJobRepos(lp)};
  jobRepos->addJob(job);

  // create queues between repository and trsnalator
  shared_ptr<TaskQueue>q2process{make_shared<TaskQueue>(10)};
  shared_ptr<TaskQueue>qprocessed{make_shared<TaskQueue>(10)};

  // create threads (producer, translator and consumer of translated segments)
  thread thr_translate{translateTasks,q2process,qprocessed};
  thread thr_schedule{scheduleTasks,jobRepos,q2process};
  thread thr_collect{collect,jobRepos,qprocessed};

  // join threads
  thr_collect.join();
  thr_schedule.join();
  thr_translate.join();

  // print job after translation
  cerr<<"JOB: "<<*job<<endl;
}

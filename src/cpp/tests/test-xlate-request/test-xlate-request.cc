#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TranslationTask.h"
#include "xlate-jobs/LanguageCode.h"
#include "xlate-jobs/TaskQueue.h"
#include "xlate-jobs/TaskCollector.h"
#include "xlate-jobs/TaskScheduler.h"
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
    cout<<"translating segment no: "<<task->segno()<<", in job: "<<task->jobid()<<", task: "<<task->id()<<", from: "<<task->lanpair().first<<" to "<<task->lanpair().second<<", text: "<<task->srcSeg()<<endl;;
    task->setTargetSeg(string("TRANSLATED: ")+task->srcSeg());

    // send translated task back to whoever wants to process it
    qprocessed->enq(task);
  }
  // send null task to stop receiver
  qprocessed->enq(task);
}
// ------- helper functions
// create a request
shared_ptr<TranslateRequest>createRequest(LanguagePair const&lp,initializer_list<string>segs){
  return make_shared<TranslateRequest>(lp,segs);
}
// main test program
int main(){
  try{
    // --- create components making up system
    LanguagePair lanpair{make_lanpair("en","sv")};
    shared_ptr<TranslationJobRepository>jobRepos{make_shared<TranslationJobRepository>(lanpair)};
    shared_ptr<TaskQueue>q2process{make_shared<TaskQueue>(10)};
    shared_ptr<TaskQueue>qprocessed{make_shared<TaskQueue>(10)};
    TaskCollector taskCollector{jobRepos,qprocessed};
    TaskScheduler taskScehduler{jobRepos,q2process};

    // --- start component - each one in a separate thread
    thread thr_translate{translateTasks,q2process,qprocessed};
    thread thr_schedule{[&](){taskScehduler();}};
    thread thr_collect{[&](){taskCollector();}};

    // --- create a job and add job to job repository
    shared_ptr<TranslateRequest>req{createRequest(lanpair,{"Hello world","second phrase","third phrase"})};
    shared_ptr<TranslationJob>job{make_shared<TranslationJob>(req)};
    cerr<<"JOB: "<<*job<<endl;
    jobRepos->addJob(job);

    // create a job having a null ptr task - will teminate processing
// NOTE!

    // --- done
    // join threads
    thr_collect.join();
    thr_schedule.join();
    thr_translate.join();

    // print job after translation
    cerr<<"JOB: "<<*job<<endl;
  }
  catch(exception const&e){
    cerr<<"cought exception: "<<e.what()<<endl;
  }
}

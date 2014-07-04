#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TranslationTask.h"
#include "xlate-jobs/LanguageCode.h"
#include "xlate-jobs/TaskQueue.h"
#include "xlate-jobs/TaskCollector.h"
#include "xlate-jobs/TaskScheduler.h"
#include "xlate-jobs/DummyTranslationEngine.h"
#include <boost/log/trivial.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <utility>
#include <thread>
#include <chrono>
using namespace std;
using namespace xlate;

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
    DummyTranslationEngine translator{q2process,qprocessed};

    // --- start component - each one in a separate thread
    thread thr_translate{[&](){translator();}};
    thread thr_schedule{[&](){taskScehduler();}};
    thread thr_collect{[&](){taskCollector();}};

    // --- create a job and add job to job repository
    shared_ptr<TranslateRequest>req{createRequest(lanpair,{"Hello world","second phrase","third phrase"})};
    shared_ptr<TranslationJob>job{make_shared<TranslationJob>(req)};
    BOOST_LOG_TRIVIAL(debug)<<"JOB: "<<*job;
    jobRepos->addJob(job);

    // sleep a little and then terminate scheduler
    BOOST_LOG_TRIVIAL(debug)<<"sleeping 1 second ...";
    ::this_thread::sleep_for(chrono::milliseconds(1));
    BOOST_LOG_TRIVIAL(debug)<<"terminating scheduler";
    taskScehduler.terminate();

    // --- done
    // join threads
    thr_collect.join();
    thr_schedule.join();
    thr_translate.join();

    // print job after translation
    BOOST_LOG_TRIVIAL(debug)<<"JOB: "<<*job;
  }
  catch(exception const&e){
    cerr<<"cought exception: "<<e.what()<<endl;
  }
}

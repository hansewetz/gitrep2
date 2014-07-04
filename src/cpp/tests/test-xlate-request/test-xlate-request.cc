#include "xlate-jobs/LanguageCode.h"
#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/DummyTranslatorCt.h"

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
    // --- create a language repository
    BOOST_LOG_TRIVIAL(debug)<<"creating job repository ...";
    LanguagePair lanpair{make_lanpair("en","sv")};
    shared_ptr<TranslationJobRepository>jobRepos{make_shared<TranslationJobRepository>(lanpair)};

    // -- create a language transation component and start it
    BOOST_LOG_TRIVIAL(debug)<<"creating translator ...";
    DummyTranslatorCt xlator(jobRepos);
    thread thr_xlate{[&](){xlator();}};

    // --- create a request and then a job and add job to job repository
    BOOST_LOG_TRIVIAL(debug)<<"adding jobs to job repository ...";
    shared_ptr<TranslateRequest>req{createRequest(lanpair,{"Hello world","second phrase","third phrase"})};
    shared_ptr<TranslationJob>job{make_shared<TranslationJob>(req)};
    BOOST_LOG_TRIVIAL(debug)<<"JOB: "<<*job;
    jobRepos->addJob(job);

    // -- sleep a little and then terminate translator
    BOOST_LOG_TRIVIAL(debug)<<"sleeping 1 milli second ...";
    ::this_thread::sleep_for(chrono::milliseconds(1));
    BOOST_LOG_TRIVIAL(debug)<<"terminating translator ...";
    xlator.terminate();

    // -- join translation thread
    BOOST_LOG_TRIVIAL(debug)<<"joining xlate thread ...";
    thr_xlate.join();

    // -- print job after translation
    BOOST_LOG_TRIVIAL(debug)<<"JOB: "<<*job;
  }
  catch(exception const&e){
    cerr<<"cought exception: "<<e.what()<<endl;
  }
}

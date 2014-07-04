#include "xlate-jobs/LanguageCode.h"
#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJob.h"
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
    // --- make language pair for this translator and also for test requests
    LanguagePair lanpair{make_lanpair("en","sv")};

    // -- create a language transation component and start it
    DummyTranslatorCt xlator(lanpair);
    thread thr_xlate{[&](){xlator();}};

    // --- create a request and then a job and add job to for translation
    shared_ptr<TranslateRequest>req{createRequest(lanpair,{"Hello world","second phrase","third phrase"})};
    shared_ptr<TranslationJob>job{make_shared<TranslationJob>(req)};
    BOOST_LOG_TRIVIAL(debug)<<"JOB (before translation): "<<*job;
    xlator.addJob(job);

    // -- sleep a little and then terminate translator
    ::this_thread::sleep_for(chrono::milliseconds(1));
    xlator.terminate();

    // -- join translation thread
    thr_xlate.join();

    // -- print job after translation
    BOOST_LOG_TRIVIAL(debug)<<"JOB (after translation): "<<*job;
  }
  catch(exception const&e){
    cerr<<"cought exception: "<<e.what()<<endl;
  }
}

#include "xlate-jobs/DummyTranslatorCt.h"
#include "xlate-jobs/TaskQueue.h"
#include "xlate-jobs/TaskCollector.h"
#include "xlate-jobs/TaskScheduler.h"
#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/DummyEngine.h"
#include <iostream>
#include <thread>

using namespace std;
namespace xlate{
// ctor
DummyTranslatorCt::DummyTranslatorCt(LanguagePair const&lanpair):
    jobrep_(make_shared<TranslationJobRepository>(lanpair)),
    qin_(make_shared<TaskQueue>(10)), // NOTE! Hard coded limit queue
    qout_(make_shared<TaskQueue>(10)),
    scheduler_(make_shared<TaskScheduler>(jobrep_,qin_)),
    collector_(make_shared<TaskCollector>(jobrep_,qout_)),
    engine_(make_shared<DummyEngine>(qin_,qout_)){
}
// run component
void DummyTranslatorCt::operator()(){
  // start threads for scheduler, collector and translator
  thread thr_schedule{[&](){(*scheduler_)();}};
  thread thr_collect{[&](){(*collector_)();}};
  thread thr_engine{[&](){(*engine_)();}};

  // collect threads ones they ar edone (i.e., received a null task)
  thr_engine.join();
  thr_collect.join();
  thr_schedule.join();
}
void DummyTranslatorCt::terminate(){
  // terminate through scheduler
  lock_guard<mutex>lock(mtx_);
  scheduler_->terminate();
}
// add job to be translated
void DummyTranslatorCt::addJob(std::shared_ptr<TranslationJob>job){
  lock_guard<mutex>lock(mtx_);
  jobrep_->addJob(job);
}
}

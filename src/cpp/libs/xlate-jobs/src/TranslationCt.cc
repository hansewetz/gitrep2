#include "xlate-jobs/TranslationCt.h"
#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TaskScheduler.h"
#include "xlate-jobs/EngineProxy.h"
using namespace std;
namespace xlate{

// ctor
TranslationCt::TranslationCt(boost::asio::io_service&ios,size_t maxScheduledJobs,size_t maxEngines):
    ios_(ios),
    qschedTaskSize_(maxEngines),qschedJobSize_(maxScheduledJobs),
    maxEngines_(maxEngines),
    qnewJob_{make_shared<JobQueue>(qnewJobSize_)},qschedJob_{make_shared<JobQueue>(qschedJobSize_)},
    qschedTask_{make_shared<TaskQueue>(qschedTaskSize_)},qtransTasks_{make_shared<TaskQueue>(qtransTasksSize_)},
    jobrep_{make_shared<TranslationJobRepository>(ios_,qnewJob_,qschedJob_,qtransTasks_)},
    scheduler_{make_shared<TaskScheduler>(ios_,qschedJob_,qschedTask_)}{

  // create engines and start running each engine in a separate thread
  for(size_t i=0;i<maxEngines_;++i){
    std::shared_ptr<EngineProxy>engine{make_shared<EngineProxy>(qschedTask_,qtransTasks_)};
    engines_.push_back(engine);
    std::thread thr(&EngineProxy::run,engine);
    thr_engines_.push_back(std::move(thr));
  }
}
// dtor
TranslationCt::~TranslationCt(){
  // join engine threads
  for(auto&thr:thr_engines_){
    if(thr.joinable())thr.join();
  }
}
// start translation componnet
void TranslationCt::run(){
  jobrep_->run();
  scheduler_->run();
}
// get job queue for new jobs
std::shared_ptr<JobQueue>TranslationCt::getNewJobQueue()const{
  return qnewJob_;
}
}

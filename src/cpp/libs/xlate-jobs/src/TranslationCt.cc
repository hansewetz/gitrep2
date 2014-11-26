// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "xlate-jobs/TranslationCt.h"
#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TaskScheduler.h"
#include "xlate-jobs/EngineProxy.h"
#include "xlate-jobs/EngineEnv.h"
using namespace std;
namespace xlate{

// ctor
TranslationCt::TranslationCt(boost::asio::io_service&ios,size_t maxScheduledJobs,size_t maxEngines,shared_ptr<EngineEnv>enngineenv):
    ios_(ios),
    qschedTaskSize_(maxEngines),qschedJobSize_(maxScheduledJobs),
    maxEngines_(maxEngines),
    qnewJob_{make_shared<JobQueue>(qnewJobSize_)},
    qschedJob_{make_shared<JobQueue>(qschedJobSize_)},
    qschedTask_{make_shared<TaskQueue>(qschedTaskSize_)},
    qtransTasks_{make_shared<TaskQueue>(qtransTasksSize_)},
    qtransJobs_{make_shared<JobQueue>(qtransJobSize_)},
    jobrep_{make_shared<TranslationJobRepository>(ios_,qnewJob_,qschedJob_,qtransTasks_,qtransJobs_)},
    scheduler_{make_shared<TaskScheduler>(ios_,qschedJob_,qschedTask_)},enngineenv_(enngineenv){

  // create engines and start running each engine in a separate thread
  for(size_t i=0;i<maxEngines_;++i){
    std::shared_ptr<EngineProxy>engine{make_shared<EngineProxy>(ios_,qschedTask_,qtransTasks_,enngineenv_)};
    engines_.push_back(engine);
  }
}
// dtor
TranslationCt::~TranslationCt(){
}
// start translation component
void TranslationCt::run(){
  jobrep_->run();
  scheduler_->run();
  for(auto e:engines_)e->run();
}
// get #of jobs in system
size_t TranslationCt::size()const{
  return jobrep_->size();
}
// get job queue for new jobs
std::shared_ptr<JobQueue>TranslationCt::getNewJobQueue()const{
  return qnewJob_;
}
// get queue with translated jobs
std::shared_ptr<JobQueue>TranslationCt::getTranslatedJobQueue()const{
  return qtransJobs_;
}
}

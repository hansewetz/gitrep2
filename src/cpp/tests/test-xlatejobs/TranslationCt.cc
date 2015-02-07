// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "TranslationCt.h"
#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TaskScheduler.h"
#include "xlate-jobs/EngineProxy.h"
#include "xlate-jobs/EngineEnv.h"
#include <boost/log/trivial.hpp>
#include "asio-extensions/detail/queue_interface_base.hpp"
using namespace std;
namespace xlate{

// ctor
TranslationCt::TranslationCt(boost::asio::io_service&ios,size_t maxScheduledJobs,size_t maxEngines,shared_ptr<EngineEnv>enngineenv):
    ios_(ios),
    maxEngines_(maxEngines),
    enngineenv_(enngineenv),
    qschedTaskSize_(maxEngines),
    qschedJobSize_(maxScheduledJobs),
    qnewJob_{shared_ptr<ConcreteJobQueue>(new ConcreteJobQueue(qnewJobSize_))},
    qschedJob_{shared_ptr<ConcreteJobQueue>(new ConcreteJobQueue(qschedJobSize_))},
    qschedTask_{shared_ptr<ConcreteTaskQueue>(new ConcreteTaskQueue(qschedTaskSize_))},
    qtransTasks_{shared_ptr<ConcreteTaskQueue>(new ConcreteTaskQueue(qtransTasksSize_))},
    qtransJobs_{shared_ptr<ConcreteJobQueue>(new ConcreteJobQueue(qtransJobSize_))},
    jobrep_{make_shared<TranslationJobRepository>(ios_,qnewJob_,qschedJob_,qtransTasks_,qtransJobs_)},
    scheduler_{make_shared<TaskScheduler>(ios_,qschedJob_,qschedTask_)}{

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
// stop component
void TranslationCt::stop(){
  // stop engines
  for(auto e:engines_)e->stop();

  // stop all queues - will send interup to all agent registered with queue
  // (interup in each component should not re-register with asio)
  BOOST_LOG_TRIVIAL(debug)<<"TranslationCt::stop - stopping qnewJob ...";
  qnewJob_->disable_enq(true);
  qnewJob_->disable_deq(true);

  BOOST_LOG_TRIVIAL(debug)<<"TranslationCt::stop - stopping qschedJob ...";
  qschedJob_->disable_enq(true);
  qschedJob_->disable_deq(true);

  BOOST_LOG_TRIVIAL(debug)<<"TranslationCt::stop - stopping qschedTask ...";
  qschedTask_->disable_enq(true);
  qschedTask_->disable_deq(true);

  BOOST_LOG_TRIVIAL(debug)<<"TranslationCt::stop - stopping qtransTasks ...";
  qtransTasks_->disable_enq(true);
  qtransTasks_->disable_deq(true);

  BOOST_LOG_TRIVIAL(debug)<<"TranslationCt::stop - stopping qtransJobs ...";
  qtransJobs_->disable_enq(true);
  qtransJobs_->disable_deq(true);

  BOOST_LOG_TRIVIAL(debug)<<"TranslationCt::stop - all queues stopped";
}
// get #of jobs in system
size_t TranslationCt::size()const{
  return jobrep_->size();
}
// get job queue for new jobs
std::shared_ptr<ConcreteJobQueue>TranslationCt::getNewJobQueue()const{
  return qnewJob_;
}
// get queue with translated jobs
std::shared_ptr<ConcreteJobQueue>TranslationCt::getTranslatedJobQueue()const{
  return qtransJobs_;
}
}

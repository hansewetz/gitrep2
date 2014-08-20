#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TranslationTask.h"
#include "utils/utility.h"
#include <boost/log/trivial.hpp>
#include <iostream>
#include <functional>
using namespace std;
using namespace std::placeholders;
namespace xlate{

// ctor (setup queue listeners and queue senders)
TranslationJobRepository::TranslationJobRepository(boost::asio::io_service&ios,std::shared_ptr<JobQueue>qnew,
                                                   std::shared_ptr<JobQueue>qsched,std::shared_ptr<TaskQueue>qtask):
    ios_(ios),qnewListener_{make_shared<JobQueueListener>(ios_,qnew)},
    qschedSender_{make_shared<JobQueueSender>(ios_,qsched)},qtaskListener_{make_shared<TaskQueueListener>(ios_,qtask)},
    waiting4unblock_(true){
}
// start listening on events
void TranslationJobRepository::run(){
  waitForUnblock();
  waitForNewJob();
  waitForTranslatedTask();
}
// wait for scheduler queue to unblock
void TranslationJobRepository::waitForUnblock(){
  BOOST_LOG_TRIVIAL(debug)<<"TranslationJobRepository::waitForUnblock - enabling unblock events";
  waiting4unblock_=true;
  qschedSender_->async_wait_enq(std::bind(&TranslationJobRepository::waitUnblockHandler,this,_1));
}
// wait for new job
void TranslationJobRepository::waitForNewJob(){
  BOOST_LOG_TRIVIAL(debug)<<"TranslationJobRepository::waitForNewJob - enabling job events";
  qnewListener_->async_deq(std::bind(&TranslationJobRepository::newJobHandler,this,_1,_2));
}
// wait for translated task
void TranslationJobRepository::waitForTranslatedTask(){
  BOOST_LOG_TRIVIAL(debug)<<"TranslationJobRepository::waitForTranslatedTask - enabling job events";
  qtaskListener_->async_deq(std::bind(&TranslationJobRepository::translatedTaskHandler,this,_1,_2));
}
// handler for waiting for scheduler queue to be unblocked
// (only place from which we send jobs to scheduler)
void TranslationJobRepository::waitUnblockHandler(boost::system::error_code const&ec){
  BOOST_LOG_TRIVIAL(debug)<<"TranslationJobRepository::waitUnblockHandler - got unblock event";

  // we are no longer waiting for unblock event
  waiting4unblock_=false;

  // send one job to scheduler if we have jobs
  if(!newJobs_.empty()){
    // send job to secheduler
    std::shared_ptr<TranslationJob>job{newJobs_.front()};
    newJobs_.pop_front();
    BOOST_LOG_TRIVIAL(debug)<<"TranslationJobRepository::waitUnblockHandler - enq into scheduler queue";
    qschedSender_->sync_enq(job);

    // insert job into scheduled jobs
    schedJobs_.insert(make_pair(job->id(),job));

    // wait for scheduler queue to unblock again only if there are no jobs
    if(!newJobs_.empty())waitForUnblock();
  }
}
// handler for new job queue
// (only collects jobs and enables waiting for unblock event)
void TranslationJobRepository::newJobHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationJob>job){
  BOOST_LOG_TRIVIAL(debug)<<"TranslationJobRepository::newJobHandler - got job event: "<<*job;

  // save job and wait for new job
  newJobs_.push_back(job);
  waitForNewJob();

  // check if we should wait for unblock event from scheduler queue
  if(!waiting4unblock_)waitForUnblock();
}
// handle received translated task 
void TranslationJobRepository::translatedTaskHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task){
  BOOST_LOG_TRIVIAL(debug)<<"TranslationJobRepository::translatedTaskHandler - got task event: "<<*task;

  // lookup job for task in schedJobs_
  TranslationJobId jobid{task->jobid()};
  auto it=schedJobs_.find(jobid);
  std::shared_ptr<TranslationJob>job{it->second};
  if(it==schedJobs_.end()){
    BOOST_LOG_TRIVIAL(warning)<<"TranslationJobRepository::translatedTaskHandler: failed looking up job for translated task: "<<*task;
  }else{
    // add translated task to job and check if job is done
    job->addTranslatedTask(task);
    if(job->done()){
      ++ndone_;
      BOOST_LOG_TRIVIAL(info)<<"TranslationJobRepository::translatedTaskHandler - job with id: "<<job->id()<<" is done, ndone: "<<ndone_;

      // NOTE! Not yet done
      // ...
    }
  }
  // continue and wait for translated tasks
  waitForTranslatedTask();
}
}

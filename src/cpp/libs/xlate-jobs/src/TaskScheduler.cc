// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include "xlate-jobs/TaskScheduler.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TranslationTask.h"
#include "utils/utility.h"
#include <boost/log/trivial.hpp>
using namespace std;
using namespace std::placeholders;
namespace xlate{

// ctor
TaskScheduler::TaskScheduler(boost::asio::io_service&ios,std::shared_ptr<JobQueueDeq>qjob,std::shared_ptr<TaskQueueEnq>qtask):
    ios_(ios),
    qjobListener_(make_shared<JobQueueListener>(ios,qjob.get())),
    qtaskSender_(make_shared<TaskQueueSender>(ios,qtask.get())),
     waiting4unblock_(true){
}
// start listening on events
void TaskScheduler::run(){
  waitForUnblock();
  waitForNewJob();
}
// wait for task queue to unblock
void TaskScheduler::waitForUnblock(){
  BOOST_LOG_TRIVIAL(debug)<<"TaskScheduler::waitForUnblock - enabling unblock events";
  waiting4unblock_=true;
  qtaskSender_->async_wait_enq(std::bind(&TaskScheduler::waitUnblockHandler,this,_1));
}
// wait for new job
void TaskScheduler::waitForNewJob(){
  BOOST_LOG_TRIVIAL(debug)<<"TaskScheduler::waitForNewJob - enabling job events";
  qjobListener_->async_deq(std::bind(&TaskScheduler::newJobHandler,this,_1,_2));
}
// handle unblock event
// (only place from which we send tasks)
void TaskScheduler::waitUnblockHandler(boost::system::error_code const&ec){
  // check for aborted/errors
  if(ec==boost::asio::error::operation_aborted){
    // we'll stop listening
    BOOST_LOG_TRIVIAL(debug)<<"TaskScheduler::waitUnblockHandler - unbocking of queue to engine proxy aborted ...";
    return;
  }
  if(ec!=boost::system::error_code()){
    // we'll stop listening
    BOOST_LOG_TRIVIAL(debug)<<"TaskScheduler::waitUnblockHandler - unblocking of queue to engine proxy error ...";
    return;
  }

  BOOST_LOG_TRIVIAL(debug)<<"TaskScheduler::waitUnblockHandler - got unblock event";

  // we are no longer waiting for unblock event
  waiting4unblock_=false;

  // send one task to task queue (if we have one)
  std::shared_ptr<TranslationTask>task{nextTask()};
  if(task){
    BOOST_LOG_TRIVIAL(debug)<<"TaskScheduler::waitUnblockHandler - enq into task queue";
    boost::system::error_code ec1;
    qtaskSender_->sync_enq(task,ec1);
    if(ec1==boost::asio::error::operation_aborted){
      // we'll stop listening
      BOOST_LOG_TRIVIAL(debug)<<"TaskScheduler::waitUnblockHandler - sync enq into engine task queue aborted ...";
      return;
    }
    if(ec1!=boost::system::error_code()){
      // we'll stop listening
      BOOST_LOG_TRIVIAL(debug)<<"TaskScheduler::waitUnblockHandler - sync enq into engine task queue error, ec: "<<ec1.message();
      return;
    }
  }
  // if we have a next task, then wait for task queue to unblock
  if(hasNextTask())waitForUnblock();
}
// handle a new job
// (only collects jobs and enables waiting for unblock event)
void TaskScheduler::newJobHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationJob>job){
  // check for aborted/errors
  if(ec==boost::asio::error::operation_aborted){
    // we'll stop listening
    BOOST_LOG_TRIVIAL(debug)<<"TaskScheduler::newJobHandler - receiption of new job into scheduler aborted ...";
    return;
  }
  if(ec!=boost::system::error_code()){
    // we'll stop listening
    BOOST_LOG_TRIVIAL(debug)<<"TaskScheduler::newJobHandler - receiption of new job into scheduler error, ec: "<<ec.message();
    return;
  }
  BOOST_LOG_TRIVIAL(debug)<<"TaskScheduler::newJobHandler - got job event: "<<*job;

  // save job and wait for new job (only save job if it has tasks)
  if(job->noUntranslated()>0){
    jobs_.push_back(job);
  }else{
    BOOST_LOG_TRIVIAL(warning)<<"TaskScheduler::newJobHandler: scheduler received job with 0 tasks, jobid: "<<job->id();
  }
  waitForNewJob();

  // check if we should wait for unblock event from task queue
  if(!waiting4unblock_)waitForUnblock();
}
// get next task (return nullptr of none available)
// (invariant: all jobs in jobs_ have tasks)
std::shared_ptr<TranslationTask>TaskScheduler::nextTask(){
  // get task from first job, remove job if no more tasks in job and return task
  if(jobs_.empty())return std::shared_ptr<TranslationTask>(nullptr);

  // get job to pull task from
  std::shared_ptr<TranslationJob>job{jobs_.front()};
  std::shared_ptr<TranslationTask>ret{job->getNextTask()};
  
  // remove job if it is done, else move it to back of queue
  if(job->noUntranslated()==0)jobs_.pop_front();
  else{
    jobs_.pop_front();
    jobs_.push_back(job);
  }
  // return task to process
  return ret;
}
// check if there is a next task
// (invariant: all jobs in jobs_ have tasks)
bool TaskScheduler::hasNextTask()const{
  if(jobs_.empty())return false;
  return true;
}
}

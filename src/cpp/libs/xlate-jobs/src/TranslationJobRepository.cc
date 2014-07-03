#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TranslationTask.h"
#include "xlate-jobs/LanguageCode.h"
#include "utils/utility.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctor
TranslationJobRepository::TranslationJobRepository(LanguagePair const&lp):lp_(lp){
}
// add task to job in repository
// (return true if task added, else false)
void TranslationJobRepository::addCompletedTask(shared_ptr<TranslationTask>task){
  // lookup job and add task to job
  shared_ptr<TranslationJob>job{getStartedJob(task->jobid())};
  job->addTranslatedTask(task);
}
// repository update functions
void TranslationJobRepository::addJob(shared_ptr<TranslationJob>job){
  // insert job in jobsPending_
  unique_lock<mutex>lock(mtx_);
  idleJobs_.push_back(job);
  cond_.notify_all();
}
// get job for processing
shared_ptr<TranslationJob>TranslationJobRepository::startJob(){
  // block until we have a job
  unique_lock<mutex>lock(mtx_);
  cond_.wait(lock,[&](){return !idleJobs_.empty();});

  // remove from idelJobs_ and insert into startedJobs_
  shared_ptr<TranslationJob>job{idleJobs_.front()};
  idleJobs_.pop_front();
  startedJobs_.insert(make_pair(job->id(),job));
  return job;
}
// get a started job from jobid
shared_ptr<TranslationJob>TranslationJobRepository::getStartedJob(TranslationJobId const&jobid){
  lock_guard<mutex>lock(mtx_);
  return getStartedJobNoLock(jobid);
}
// remove job from repository
shared_ptr<TranslationJob>TranslationJobRepository::removeStartedJob(TranslationJobId const&jobid){
  // check if job is a started
  auto it=startedJobs_.find(jobid);
  if(it!=startedJobs_.end()){
    auto ret{it->second};
    startedJobs_.erase(it);
    return it->second;
  }
  THROW_RUNTIME("attempt to remove idle job as started job, jobid: "<<jobid);
}
// print function
ostream&TranslationJobRepository::print(ostream&os)const{
  lock_guard<mutex>lock(mtx_);
  return os<<"lanpair:: "<<lp_<<endl;
}
// get a started job from jobid - no locks
shared_ptr<TranslationJob>TranslationJobRepository::getStartedJobNoLock(TranslationJobId const&jobid){
  // lookup job among started jobs
  auto it=startedJobs_.find(jobid);
  if(it==startedJobs_.end())THROW_RUNTIME("attempt to retrieve non-existing started job with job id: "<<jobid);
  return it->second;
}
// debug print function
ostream&operator<<(ostream&os,TranslationJobRepository const&r){
  return r.print(os);
}
}

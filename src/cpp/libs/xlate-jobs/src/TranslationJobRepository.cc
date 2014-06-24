#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/LanguageCode.h"
#include "utils/utility.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctor
TranslationJobRepository::TranslationJobRepository(LanguagePair const&lp):lp_(lp){
}
// repository update functions
void TranslationJobRepository::addJob(shared_ptr<TranslationJob>job){
  // insert job in jobsPending_
  unique_lock<mutex>lock(mtx_);
  idleJobs_.push_back(job);
  cond_.notify_all();
}
// get job for translation
shared_ptr<TranslationJob>TranslationJobRepository::getJobForTranslation(){
  // block until we have a job
  unique_lock<mutex>lock(mtx_);
  cond_.wait(lock,[&](){return !idleJobs_.empty();});

  // remove from idelJobs_ and insert into startedJobs_
  shared_ptr<TranslationJob>job{idleJobs_.front()};
  idleJobs_.pop_front();
  startedJobs_.insert(make_pair(job->id(),job));
  return job;
}
// print function
ostream&TranslationJobRepository::print(ostream&os)const{
  lock_guard<mutex>lock(mtx_);
  return os<<"lanpair:: "<<lp_<<endl;
}
// debug print function
ostream&operator<<(ostream&os,TranslationJobRepository const&r){
  return r.print(os);
}
}

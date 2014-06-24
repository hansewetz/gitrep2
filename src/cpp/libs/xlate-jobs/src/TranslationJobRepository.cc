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
  waitingJobs_.push_back(job);
  cond_.notify_all();
}
// get job for translation
shared_ptr<TranslationJob>TranslationJobRepository::getJobForTranslation(){
  // block until we have a job
  unique_lock<mutex>lock(mtx_);
  cond_.wait(lock,[&](){return !waitingJobs_.empty();});

  // move job from waitingJobs_ --> processingJobs_ and return job
  shared_ptr<TranslationJob>job{waitingJobs_.front()};
  waitingJobs_.pop_front();
  processingJobs_.push_back(job);
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

#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/LanguageCode.h"
#include "utils/utility.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctor
TranslationJobRepository::TranslationJobRepository(){
}
// get #of jobs for a language pair
size_t TranslationJobRepository::jobCount(LanguagePair const&lp)const{
  lock_guard<mutex>lock(mtx_);
  return lanp2jobmap_.count(lp);
}
// repository update functions
void TranslationJobRepository::addJob(shared_ptr<TranslationJob>job){
  unique_lock<mutex>lock(mtx_);

  // insert job in 'allJob map
  allJobs_.insert(make_pair(job->id(),job));

  // insert jobs in language pair quuee (if key is already in map, no insert is done)
  auto it=lanp2jobmap_.find(job->lanpair());
  if(it!=lanp2jobmap_.end()){
    // insert new language pair queue
    auto it=lanp2jobmap_.insert(make_pair(job->lanpair(),queue<shared_ptr<TranslationJob>>()));
  }
  queue<shared_ptr<TranslationJob>>&q=it->second;
  q.push(job);

  // notify waiing consumers
  cond_.notify_all();
}
// get job for translation
shared_ptr<TranslationJob>TranslationJobRepository::getJobForTranslation(TranslationJobId const&jobid){
  // move job form allJobs --> startedJobs
  lock_guard<mutex>lock(mtx_);
  auto it=allJobs_.find(jobid);
  if(it==allJobs_.end())THROW_RUNTIME("attempt to lookup job: "<<jobid<<" for translation");
  startedJobs_.insert(*it);
  allJobs_.erase(it);
}
// remove job and get job objects
shared_ptr<TranslationJob>TranslationJobRepository::removeJob(TranslationJobId const&){
  lock_guard<mutex>lock(mtx_);
  // NOTE! Not yet done
  // must remove to all 3 maps
  cond_.notify_all();
}
// print function
ostream&TranslationJobRepository::print(ostream&os)const{
  lock_guard<mutex>lock(mtx_);
  return os<<"#lanpairs: "<<allJobs_.size()<<endl;
}
// debug print function
ostream&operator<<(ostream&os,TranslationJobRepository const&r){
  return r.print(os);
}
}

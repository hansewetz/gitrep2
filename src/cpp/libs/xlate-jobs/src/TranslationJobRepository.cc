#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/LanguageCode.h"
#include <iostream>
using namespace std;
namespace xlate{

// NOTE! Must use mutex + condition variable since
//	 there are produceres and consumers of jobs for the rep[osityory

// ctor
TranslationJobRepository::TranslationJobRepository(){
}
// check if job exist
bool TranslationJobRepository::hasJob(TranslationJobId const&jobid)const{
  return id2jobmap_.find(jobid)!=id2jobmap_.end();
}
// get job by job id
shared_ptr<TranslationJob>TranslationJobRepository::getJob(TranslationJobId const&jobid)const{
  auto it=id2jobmap_.find(jobid);
  if(it==id2jobmap_.end())return shared_ptr<TranslationJob>(nullptr);
  return it->second;
}
// get #of jobs for a language pair
size_t TranslationJobRepository::jobCount(LanguagePair const&lp)const{
  return lanp2jobmap_.count(lp);
}
// repository update functions
void TranslationJobRepository::addJob(shared_ptr<TranslationJob>){
  // NOTE! Not yet done
  // must add to both maps
}
// remove job and get job objects
shared_ptr<TranslationJob>TranslationJobRepository::removeJob(TranslationJobId const&)const{
  // NOTE! Not yet done
  // must add to both maps
}
// print function
ostream&TranslationJobRepository::print(ostream&os)const{
  return os<<"#lanpairs: "<<id2jobmap_.size()<<endl;
}
// debug print function
ostream&operator<<(ostream&os,TranslationJobRepository const&r){
  return r.print(os);
}
}

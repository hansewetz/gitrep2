#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/LanguageCode.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctor
TranslationJobRepository::TranslationJobRepository(){
}
// get job by job id
shared_ptr<TranslationJob>TranslationJobRepository::getJob(TranslationJobId const&jobid)const{
  // NOTE! Not yet done
}
// get #of jobs for a language pair
size_t TranslationJobRepository::jobCount(LanguagePair const&)const{
  // NOTE! Not yet done
}
// repository update functions
void TranslationJobRepository::addJob(shared_ptr<TranslationJob>){
  // NOTE! Not yet done
}
// print function
ostream&TranslationJobRepository::print(ostream&os)const{
  return os<<"#lanpairs: "<<jobs_.size()<<endl;
}
// debug print function
ostream&operator<<(ostream&os,TranslationJobRepository const&r){
  return r.print(os);
}
}

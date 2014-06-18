#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationTask.h"
#include "utils/Id.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctors
TranslationJob::TranslationJob(TranslateRequest const&req):
    id_(TranslationJobId()),
    slan_(req.srcLan()),tlan_(req.targLan()){

  // create tasks
  vector<string>const&segs{req.segs()};
  size_t segcount{0};
  for(auto s:segs){
    shared_ptr<TranslationTask>task{new TranslationTask(slan_,tlan_,s,segcount++)};
    tasks_.insert(make_pair(false,task));
  }
}
// get job id
TranslationJobId const&TranslationJob::id()const{
  return id_;
}
// get source lan
LanguageCode const&TranslationJob::srcLan()const{
  return slan_;
}
// get target lan
LanguageCode const&TranslationJob::targLan()const{
  return tlan_;
}
// get list of all tasks
vector<shared_ptr<TranslationTask>>TranslationJob::tasks()const{
  vector<shared_ptr<TranslationTask>>ret;
  for(auto const&p:tasks_)ret.push_back(p.second);
  return ret;
}
// book keeping functions
shared_ptr<TranslationTask>TranslationJob::nextTask()const{
  // simple algorithm - pick next segment
  // NOTE!
}
// print function
ostream&TranslationJob::print(ostream&os)const{
  return os<<"id: "<<id_<<", source-lan: "<<slan_<<", target-lan: "<<tlan_<<", #tasks: "<<tasks_.size();
}
// print operator
ostream&operator<<(ostream&os,TranslationJob const&j){
  return j.print(os);
}
}

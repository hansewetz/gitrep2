#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationTask.h"
#include "utils/Id.h"
#include "utils/utility.h"
#include <iostream>
#include <algorithm>
using namespace std;
namespace xlate{

// ctors
TranslationJob::TranslationJob(TranslateRequest const&req):
    id_(TranslationJobId()),
    slan_(req.srcLan()),tlan_(req.targLan()){

  // create tasks to be translated
  vector<string>const&segs{req.segs()};
  size_t segcount{0};
  for(auto s:segs){
    auto task=make_shared<TranslationTask>(slan_,tlan_,s,segcount++);
    nonTranslated_.push_back(task);
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
// get #of translated segments
size_t TranslationJob::noTranslated()const{
  return translated_.size();
}
// get #of un-translated segments
size_t TranslationJob::noUntranslated()const{
  return nonTranslated_.size();
}
// get #of in translation segments
size_t TranslationJob::noInTranslation()const{
  return inTranslation_.size();
}
// book keeping functions
shared_ptr<TranslationTask>TranslationJob::getNextTask(){
  // check if we have anyhthing to translate
  if(nonTranslated_.size()==0)return shared_ptr<TranslationTask>(nullptr);

  // get task to translate
  shared_ptr<TranslationTask>ret{*nonTranslated_.begin()};
  nonTranslated_.pop_front();

  // track sements which are in the process of translation
  TranslationTaskId id{ret->id()};
  if(inTranslation_.find(id)!=inTranslation_.end()){
    THROW_RUNTIME("attempt to insert segmementg in map of segments waiting for translation when segent already exist, id: "<<id);
  }
  inTranslation_[id]=ret;

  //inTranslation_
  return ret;
}
// add a translated task
void TranslationJob::addTranslatedTask(std::shared_ptr<TranslationTask>task){
  // make sure the task is waiting for translation
  TranslationTaskId id{task->id()};
  decltype(inTranslation_)::const_iterator it{inTranslation_.find(id)};
  if(it==inTranslation_.end()){
    THROW_RUNTIME("attempt to insert translated segment in map of segments waiting for translation when segent already does not exists, id: "<<id);
  }
  // remove task from tasks waiting for translation
  inTranslation_.erase(id);

  // add task to translated tasks
  translated_.push_back(task);
}
// print function
ostream&TranslationJob::print(ostream&os)const{
  return os<<"id: "<<id_<<", source-lan: "<<slan_<<", target-lan: "<<tlan_<<", #translated: "<<noTranslated()<<", #non-translated: "<<noUntranslated()<<", #in-translation: "<<noInTranslation();
}
// print operator
ostream&operator<<(ostream&os,TranslationJob const&j){
  return j.print(os);
}
}

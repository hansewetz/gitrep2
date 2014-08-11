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
TranslationJob::TranslationJob(std::shared_ptr<TranslateRequest>req):
    id_(TranslationJobId()),
    lanpair_{req->lanpair()}{

  // create tasks to be translated
  vector<string>const&segs{req->segs()};
  size_t segcount{0};
  for(auto s:segs){
    auto task=make_shared<TranslationTask>(id_,lanpair_,s,segcount++);
    nonTranslated_.push_back(task);
  }
}
// get job id
TranslationJobId const&TranslationJob::id()const{
  return id_;
}
// get language pair
LanguagePair const&TranslationJob::lanpair()const{
  return lanpair_;
}
// get #of translated segments
size_t TranslationJob::noTranslated()const{
  lock_guard<mutex>lock(mtx_);
  return translated_.size();
}
// get #of un-translated segments
size_t TranslationJob::noUntranslated()const{
  lock_guard<mutex>lock(mtx_);
  return nonTranslated_.size();
}
// get #of in translation segments
size_t TranslationJob::noInTranslation()const{
  lock_guard<mutex>lock(mtx_);
  return inTranslation_.size();
}
// check if we are done translating
bool TranslationJob::done()const{
  lock_guard<mutex>lock(mtx_);
  return inTranslation_.size()==0&&nonTranslated_.size()==0;
}
// book keeping functions
shared_ptr<TranslationTask>TranslationJob::getNextTask(){
  lock_guard<mutex>lock(mtx_);

  // check if we have anyhthing to translate
  if(nonTranslated_.size()==0)return shared_ptr<TranslationTask>(nullptr);

  // get task to translate
  shared_ptr<TranslationTask>ret{*nonTranslated_.begin()};
  nonTranslated_.pop_front();

  // track sements which are in the process of translation
  TranslationTaskId id{ret->id()};
  if(inTranslation_.find(id)!=inTranslation_.end()){
    THROW_RUNTIME("attempt to retrieve translation task already in translation, id: "<<id);
  }
  inTranslation_[id]=ret;

  //inTranslation_
  return ret;
}
// add a translated task
void TranslationJob::addTranslatedTask(std::shared_ptr<TranslationTask>task){
  lock_guard<mutex>lock(mtx_);

  // make sure the task is waiting for translation
  TranslationTaskId id{task->id()};
  decltype(inTranslation_)::const_iterator it{inTranslation_.find(id)};
  if(it==inTranslation_.end()){
    THROW_RUNTIME("attempt to insert translated task when task is not waiting to be translated, id: "<<id);
  }
  // remove task from tasks waiting for translation
  inTranslation_.erase(id);

  // add task to translated tasks
  translated_.push_back(task);
}
// print function
ostream&TranslationJob::print(ostream&os)const{
  lock_guard<mutex>lock(mtx_);
  return os<<"id: "<<id_<<", source-lan: "<<lanpair_.first<<", target-lan: "<<lanpair_.second<<
             ", #translated: "<<translated_.size()<<
             ", #non-translated: "<<nonTranslated_.size()<<
             ", #in-translation: "<<inTranslation_.size();
}
// print operator
ostream&operator<<(ostream&os,TranslationJob const&j){
  return j.print(os);
}
}

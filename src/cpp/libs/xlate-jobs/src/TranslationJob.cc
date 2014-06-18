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
    slan_(req.srcLan()),tlan_(req.targLan()),
    noTranslated_{0},noUntranslated_{req.segs().size()},noInTranslation_(0){

  // create tasks to be translated
  vector<string>const&segs{req.segs()};
  size_t segcount{0};
  for(auto s:segs){
    nonTranslated_.push_back(make_shared<TranslationTask>(slan_,tlan_,s,segcount++));
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
  return noTranslated_;
}
// get #of un-translated segments
size_t TranslationJob::noUntranslated()const{
  return noUntranslated_;
}
// get #of in translation segments
size_t TranslationJob::noInTranslation()const{
  return noInTranslation_;
}
// book keeping functions
shared_ptr<TranslationTask>TranslationJob::nextTask(){
  // check if we have anyhthing to translate
  if(nonTranslated_.size()==0)return shared_ptr<TranslationTask>(nullptr);

  // get task to translate
  shared_ptr<TranslationTask>ret{*nonTranslated_.begin()};
  nonTranslated_.pop_front();

  // track sements which are in the process of translation
  ++noInTranslation_;
  size_t segno{ret->segno()};
  if(inTranslation_.find(segno)!=inTranslation_.end()){
    THROW_RUNTIME("attempt to insert segmementg in map of segments waiting for translation when segent already exist, segno: "<<segno);
  }
  inTranslation_[segno]=ret;

  //inTranslation_
  return ret;
}
// print function
ostream&TranslationJob::print(ostream&os)const{
  return os<<"id: "<<id_<<", source-lan: "<<slan_<<", target-lan: "<<tlan_<<", #translated: "<<noTranslated_<<", #non-translated: "<<noUntranslated_<<", #in-translation: "<<noInTranslation_;
}
// print operator
ostream&operator<<(ostream&os,TranslationJob const&j){
  return j.print(os);
}
}

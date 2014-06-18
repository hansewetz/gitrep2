#include "xlate-jobs/TranslationTask.h"
#include "utils/utility.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctor
TranslationTask::TranslationTask(LanguageCode const&slan,LanguageCode const&tlan,string const&seg,size_t segno):
  id_(TranslationTaskId()),slan_(slan),tlan_(tlan),srcSeg_(seg),segno_(segno){
}
// get id of task
TranslationTaskId  const&TranslationTask::id()const{
  return id_;
}
// get source lan
LanguageCode const&TranslationTask::srcLan()const{
  return slan_;
}
// get target lan
LanguageCode const&TranslationTask::targLan()const{
  return tlan_;
}
// get segment to translate
string const&TranslationTask::srcSeg()const{
  return srcSeg_;
}
// do we have a target segment?
bool TranslationTask::hasTargetSeg()const{
  return hasTargetSeg_;
}
// get target segment
string const&TranslationTask::targetSeg()const{
  if(!hasTargetSeg_)THROW_RUNTIME("attempt o retrieve non-existing target segment from TranslationTask::targetSeg()");
  return targetSeg_;
}
// segment number (uniqueu within a job
size_t TranslationTask::segno()const{
  return segno_;
}
// setters
void TranslationTask::setTargetSeg(string const&seg){
  hasTargetSeg_=true;
  targetSeg_=seg;
}
// print function
ostream&TranslationTask::print(ostream&os)const{
  return os<<"id: "<<id_<<", segno: "<<segno_<<", source-lan: "<<slan_<<", target-lan: "<<tlan_<<", srcSeg: \""<<srcSeg_<<"\""<<", has target: "<<boolalpha<<hasTargetSeg_<<", target-seg: \""<<targetSeg_<<"\"";
}
// print operator
ostream&operator<<(ostream&os,TranslationTask const&t){
  return t.print(os);
}
}

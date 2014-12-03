// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "xlate-jobs/TranslationTask.h"
#include "utils/utility.h"
#include <iostream>
using namespace std;
namespace xlate{

// static attr.
const string TranslationTask::TIMEOUTSTRING{"<TIMEOUT>"};

// ctor
TranslationTask::TranslationTask(TranslationJobId const&jobid,LanguagePair const&lanpair,string const&seg,size_t segno):
  jobid_(jobid),id_(TranslationTaskId()),lanpair_(lanpair),srcSeg_(seg),segno_(segno),hasTargetSeg_(false),timeout_(false){
}
// get id of task
TranslationTaskId  const&TranslationTask::id()const{
  return id_;
}
// get job id for task
TranslationJobId const&TranslationTask::jobid()const{
  return jobid_;
}
// get language pair
LanguagePair const&TranslationTask::lanpair()const{
  return lanpair_;
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
// get egine id translating this task
EngineProxyId const&TranslationTask::engineId()const{
  return engineId_;
}
// setters
void TranslationTask::setTargetSeg(string const&seg,EngineProxyId const&engineId){
  hasTargetSeg_=true;
  targetSeg_=seg;
  timeout_=false;
  engineId_=engineId;
}
// setters
void TranslationTask::setTimeout(EngineProxyId const&engineId){
  hasTargetSeg_=true;
  targetSeg_=TIMEOUTSTRING;
  timeout_=true;
  engineId_=engineId;
}
// print function
ostream&TranslationTask::print(ostream&os)const{
  return os<<"id: "<<id_<<", jobid: "<<jobid_<<", segno: "<<segno_<<", source-lan: "<<lanpair_.first<<", target-lan: "<<lanpair_.second<<", srcSeg: \""<<srcSeg_<<"\""<<", has target: "<<boolalpha<<hasTargetSeg_<<", target-seg: \""<<targetSeg_<<"\", engineId: \""<<engineId_<<"\", tmo: "<<timeout_;
}
// print operator
ostream&operator<<(ostream&os,TranslationTask const&t){
  return t.print(os);
}
}

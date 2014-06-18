#include "xlate-jobs/TranslationTask.h"
#include <iostream>
using namespace std;
namespace xlate{

// ctor
TranslationTask::TranslationTask(LanguageCode const&slan,LanguageCode const&tlan,string const&seg,size_t segno):
  id_(TranslationTaskId()),slan_(slan),tlan_(tlan),seg_(seg),segno_(segno){
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
string const&TranslationTask::seg()const{
  return seg_;
}
size_t TranslationTask::segno()const{
  return segno_;
}
// print function
ostream&TranslationTask::print(ostream&os)const{
  return os<<"id: "<<id_<<", source-lan: "<<slan_<<", target-lan: "<<tlan_<<", seg: \""<<seg_<<"\"";
}
// print operator
ostream&operator<<(ostream&os,TranslationTask const&t){
  return t.print(os);
}
}

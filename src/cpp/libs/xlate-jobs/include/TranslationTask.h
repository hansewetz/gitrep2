// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#ifndef __TRANSLATION_TASK_H__
#define __TRANSLATION_TASK_H__
#include "xlate-jobs/LanguageCode.h"
#include "xlate-jobs/Identifiers.h"
#include <iosfwd>
#include <string>
namespace xlate{

// container for translating a single segment
class TranslationTask{
public:
  // ctors, dtor
  TranslationTask(TranslationJobId const&jobid,LanguagePair const&lanpair,std::string const&seg,std::size_t segno);
  TranslationTask(TranslationTask const&)=default;
  TranslationTask(TranslationTask&&)=default;
  TranslationTask&operator=(TranslationTask const&)=default;
  TranslationTask&operator=(TranslationTask&&)=default;
  ~TranslationTask()=default;

  // getters
  TranslationTaskId const&id()const;
  TranslationJobId const&jobid()const;
  LanguagePair const&lanpair()const;
  std::string const&srcSeg()const;
  bool hasTargetSeg()const;
  std::string const&targetSeg()const;
  std::size_t segno()const;
  EngineProxyId const&engineId()const;

  // setters
  void setTargetSeg(std::string const&seg,EngineProxyId const&engineId);
  void setTimeout(EngineProxyId const&engineId);

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  TranslationTaskId id_;
  TranslationJobId jobid_;
  LanguagePair lanpair_;
  std::string srcSeg_;
  std::string targetSeg_;
  bool hasTargetSeg_;
  std::size_t segno_;
  bool timeout_;
  EngineProxyId engineId_;
  static const std::string TIMEOUTSTRING;
};
// print operator
std::ostream&operator<<(std::ostream&os,TranslationTask const&t);
}
#endif

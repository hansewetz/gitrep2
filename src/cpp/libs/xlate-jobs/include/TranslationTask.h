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
  TranslationTask(LanguagePair const&lanpair,std::string const&seg,std::size_t segno);
  TranslationTask(TranslationTask const&)=default;
  TranslationTask(TranslationTask&&)=default;
  TranslationTask&operator=(TranslationTask const&)=default;
  TranslationTask&operator=(TranslationTask&&)=default;
  ~TranslationTask()=default;

  // getters
  TranslationTaskId  const&id()const;
  LanguagePair const&lanpair()const;
  std::string const&srcSeg()const;
  bool hasTargetSeg()const;
  std::string const&targetSeg()const;
  std::size_t segno()const;

  // setters
  void setTargetSeg(std::string const&seg);

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  TranslationTaskId id_;
  LanguagePair lanpair_;
  std::string srcSeg_;
  std::string targetSeg_;
  bool hasTargetSeg_;
  std::size_t segno_;
};
// print operator
std::ostream&operator<<(std::ostream&os,TranslationTask const&t);
}
#endif

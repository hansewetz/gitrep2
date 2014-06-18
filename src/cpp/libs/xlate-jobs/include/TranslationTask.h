#ifndef __TRANSLATION_TASK_H__
#define __TRANSLATION_TASK_H__
#include "xlate-jobs/LanguageCode.h"
#include "utils/Id.h"
#include <iosfwd>
#include <string>
namespace xlate{

// request id
class TranslationTask;
using TranslationTaskId=utils::Id<TranslationTask,std::string,true>;

// container for translating a single segment
class TranslationTask{
public:
  // ctors, dtor
  TranslationTask(LanguageCode const&slan,LanguageCode const&tlan,std::string const&seg);
  TranslationTask(TranslationTask const&)=default;
  TranslationTask(TranslationTask&&)=default;
  TranslationTask&operator=(TranslationTask const&)=default;
  TranslationTask&operator=(TranslationTask&&)=default;
  ~TranslationTask()=default;

  // getters
  TranslationTaskId  const&id()const;
  LanguageCode const&srcLan()const;
  LanguageCode const&targLan()const;
  std::string const&seg()const;

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  TranslationTaskId id_;
  LanguageCode slan_;
  LanguageCode tlan_;
  std::string seg_;
};
// print operator
std::ostream&operator<<(std::ostream&os,TranslationTask const&t);
}
#endif

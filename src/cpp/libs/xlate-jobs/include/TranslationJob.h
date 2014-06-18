#ifndef __TRANSLATE_JOB_H__
#define __TRANSLATE_JOB_H__
#include "xlate-jobs/Identifiers.h"
#include "xlate-jobs/LanguageCode.h"
#include <string>
#include <iosfwd>
#include <memory>
#include <map>
#include <vector>
namespace xlate{

// forward decl
class TranslateRequest;
class TranslationTask;

// class representing a translation job (one or more segments together with data specifying what to do)
class TranslationJob{
public:
  // ctors, dtor (non-copyable, movable)
  TranslationJob(TranslateRequest const&);
  TranslationJob(TranslationJob const&)=delete;
  TranslationJob(TranslationJob&&)=default;
  TranslationJob&operator=(TranslationJob const&)=delete;
  TranslationJob&operator=(TranslationJob&&)=default;
  virtual~TranslationJob()=default;

  // getters
  TranslationJobId const&id()const;
  LanguageCode const&srcLan()const;
  LanguageCode const&targLan()const;

  // get all tasks
  std::vector<std::shared_ptr<TranslationTask>>tasks()const;

  // book keeping functions
  std::shared_ptr<TranslationTask>nextTask()const;
  

  // NOTE! Book keeping functions */
  //	- add translated segents
  //	- get segment for translation
  //	- get #of remaining segments

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  TranslationJobId id_;
  LanguageCode slan_;
  LanguageCode tlan_;
  std::multimap<bool,std::shared_ptr<TranslationTask>>tasks_;
};
// print operator
std::ostream&operator<<(std::ostream&os,TranslationJob const&j);
}
#endif

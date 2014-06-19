#ifndef __TRANSLATE_JOB_H__
#define __TRANSLATE_JOB_H__
#include "xlate-jobs/Identifiers.h"
#include "xlate-jobs/LanguageCode.h"
#include <string>
#include <iosfwd>
#include <memory>
#include <list>
#include <map>
namespace xlate{

// forward decl
class TranslateRequest;
class TranslationTask;

// class representing a translation job (one or more segments together with data specifying what to do)
// (a job keeps tasks in a priority queue where non-ranslated tasks are on top)
class TranslationJob{
public:
  // ctors, dtor (non-copyable, movable)
  TranslationJob(TranslateRequest const&);
  TranslationJob(TranslationJob const&)=delete;
  TranslationJob(TranslationJob&&)=default;
  TranslationJob&operator=(TranslationJob const&)=delete;
  TranslationJob&operator=(TranslationJob&&)=default;
  virtual~TranslationJob()=default;

  // general getters
  TranslationJobId const&id()const;
  LanguageCode const&srcLan()const;
  LanguageCode const&targLan()const;

  // translation stats
  std::size_t noTranslated()const;
  std::size_t noUntranslated()const;
  std::size_t noInTranslation()const;
  bool done()const;

  // task management functions
  std::shared_ptr<TranslationTask>getNextTask();
  void addTranslatedTask(std::shared_ptr<TranslationTask>);
  

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
  std::list<std::shared_ptr<TranslationTask>>translated_;
  std::list<std::shared_ptr<TranslationTask>>nonTranslated_;
  std::map<TranslationTaskId,std::shared_ptr<TranslationTask>>inTranslation_;
};
// print operator
std::ostream&operator<<(std::ostream&os,TranslationJob const&j);
}
#endif

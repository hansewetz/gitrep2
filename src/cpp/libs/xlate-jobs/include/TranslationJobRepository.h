#ifndef __TRANSLATION_JOB_REPOSITORY_H__
#define __TRANSLATION_JOB_REPOSITORY_H__
#include "xlate-jobs/LanguageCode.h"
#include <iosfwd>
#include <map>
#include <memory>
namespace xlate{

// forward decl
class TranslationJob;

// class holding all jobs to be translated
class TranslationJobRepository{
public:
  // ctors, dtor
  TranslationJobRepository();
  TranslationJobRepository(TranslationJobRepository const&)=delete;
  TranslationJobRepository(TranslationJobRepository&&)=default;
  TranslationJobRepository&operator=(TranslationJobRepository const&)=delete;
  TranslationJobRepository&operator=(TranslationJobRepository&&)=default;
  ~TranslationJobRepository()=default;

  // repository query functions
  // ...

  // repository update functions
  // ...

  // print function
  std::ostream&print(TranslationJobRepository const&)const;
private:
  std::map<LanguagePair,std::shared_ptr<TranslationJob>>jobs_;
};
// debug print function
std::ostream&operator<<(std::ostream&os,TranslationJobRepository const&);
}
#endif

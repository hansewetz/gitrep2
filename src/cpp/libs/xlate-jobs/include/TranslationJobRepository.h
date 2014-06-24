#ifndef __TRANSLATION_JOB_REPOSITORY_H__
#define __TRANSLATION_JOB_REPOSITORY_H__
#include "xlate-jobs/LanguageCode.h"
#include "xlate-jobs/Identifiers.h"
#include <iosfwd>
#include <memory>
#include <mutex>
#include <condition_variable>
namespace xlate{

// forward decl
class TranslationJob;

// class holding all jobs to be translated
class TranslationJobRepository{
public:
  // ctors, dtor
  TranslationJobRepository(LanguagePair const&lp);
  TranslationJobRepository(TranslationJobRepository const&)=delete;
  TranslationJobRepository(TranslationJobRepository&&)=default;
  TranslationJobRepository&operator=(TranslationJobRepository const&)=delete;
  TranslationJobRepository&operator=(TranslationJobRepository&&)=default;
  ~TranslationJobRepository()=default;

  // repository update functions
  void addJob(std::shared_ptr<TranslationJob>);
  std::shared_ptr<TranslationJob>getJobForTranslation();

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  // map of job ids to jobs (we ensure jobids are unique)
  std::list<std::shared_ptr<TranslationJob>>waitingJobs_;
  std::list<std::shared_ptr<TranslationJob>>processingJobs_;

  // language pair for this rep
  LanguagePair lp_;

  // we have multiple consumers/producuerproducers
  mutable std::mutex mtx_;
  mutable std::condition_variable cond_;
};
// debug print function
std::ostream&operator<<(std::ostream&os,TranslationJobRepository const&);
}
#endif

#ifndef __TRANSLATION_JOB_REPOSITORY_H__
#define __TRANSLATION_JOB_REPOSITORY_H__
#include "xlate-jobs/LanguageCode.h"
#include "xlate-jobs/Identifiers.h"
#include <iosfwd>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <condition_variable>
namespace xlate{

// forward decl
class TranslationJob;

// class holding all jobs to be processed
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
  std::shared_ptr<TranslationJob>startJob();
  void blockStartingJobs();
  std::shared_ptr<TranslationJob>getStartedJob(TranslationJobId const&);
  std::shared_ptr<TranslationJob>removeStartedJob(TranslationJobId const&);

  // put a completed task back into the job to which it belongs
  void addCompletedTask(std::shared_ptr<TranslationTask>);

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  // list of idle jobs and map of jobs in progress
  std::list<std::shared_ptr<TranslationJob>>idleJobs_;
  std::map<TranslationJobId,std::shared_ptr<TranslationJob>>startedJobs_;

  // language pair for repository
  LanguagePair lp_;

  // we have multiple consumers/producuerproducers
  bool blockStartingJobs_=false;
  mutable std::mutex mtx_;
  mutable std::condition_variable cond_;

  // helper functions
  std::shared_ptr<TranslationJob>getStartedJobNoLock(TranslationJobId const&);
};
// debug print function
std::ostream&operator<<(std::ostream&os,TranslationJobRepository const&);
}
#endif

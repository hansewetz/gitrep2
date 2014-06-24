#ifndef __TRANSLATION_JOB_REPOSITORY_H__
#define __TRANSLATION_JOB_REPOSITORY_H__
#include "xlate-jobs/LanguageCode.h"
#include "xlate-jobs/Identifiers.h"
#include <iosfwd>
#include <map>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
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
  std::size_t jobCount(LanguagePair const&)const;

  // repository update functions
  void addJob(std::shared_ptr<TranslationJob>);
  std::shared_ptr<TranslationJob>getJobForTranslation(TranslationJobId const&);
  std::shared_ptr<TranslationJob>removeJob(TranslationJobId const&);

  // print function
  std::ostream&print(std::ostream&os)const;
private:
  // store jobs in a set of queues indexed by LanguagePair
  std::multimap<LanguagePair,std::queue<std::shared_ptr<TranslationJob>>>lanp2jobmap_;

  // map of job ids to jobs (we ensure jobids are unique)
  std::map<TranslationJobId,std::shared_ptr<TranslationJob>>allJobs_;
  std::map<TranslationJobId,std::shared_ptr<TranslationJob>>startedJobs_;

// NOTE! Must update 'jobsInTranslation_'

  // we have multiple consumers/producuerproducers
  mutable std::mutex mtx_;
  mutable std::condition_variable cond_;
};
// debug print function
std::ostream&operator<<(std::ostream&os,TranslationJobRepository const&);
}
#endif

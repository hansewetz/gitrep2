#ifndef __TRANSLATION_JOB_REPOSITORY_H__
#define __TRANSLATION_JOB_REPOSITORY_H__
#include "xlate-jobs/LanguageCode.h"
#include "xlate-jobs/Identifiers.h"
#include "xlate-jobs/JobQueue.h"
#include <boost/asio.hpp>
#include <iosfwd>
#include <list>
#include <map>
#include <memory>
namespace xlate{

// forward decl
class TranslationJob;

// class holding all jobs to be processed
class TranslationJobRepository{
public:
  // ctors, dtor
  TranslationJobRepository(boost::asio::io_service&ios,std::shared_ptr<JobQueue>qnew,std::shared_ptr<JobQueue>qsched,LanguagePair const&lp);
  TranslationJobRepository(TranslationJobRepository const&)=delete;
  TranslationJobRepository(TranslationJobRepository&&)=default;
  TranslationJobRepository&operator=(TranslationJobRepository const&)=delete;
  TranslationJobRepository&operator=(TranslationJobRepository&&)=default;
  ~TranslationJobRepository()=default;
private:
  // asio objects
  boost::asio::io_service&ios_;
  std::shared_ptr<JobQueueListener>qnewListener_;
  std::shared_ptr<JobQueueSender>qschedSender_;

  // list of new jobs and map of jobs in progress
  std::list<std::shared_ptr<TranslationJob>>newJobs_;
  std::map<TranslationJobId,std::shared_ptr<TranslationJob>>schedJobs_;

  // language pair for repository
  LanguagePair lp_;

  // track if scheduler queue is blocked
  bool waiting4unblock_;

  // helper functions
  void waitForUnblock();
  void waitForNewJob();

  // helper functions
  void waitUnblockHandler(boost::system::error_code const&ec);
  void newJobHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationJob>job);
};
}
#endif

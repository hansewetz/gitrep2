#ifndef __TRANSLATION_JOB_REPOSITORY_H__
#define __TRANSLATION_JOB_REPOSITORY_H__
#include "xlate-jobs/Identifiers.h"
#include "xlate-jobs/JobQueue.h"
#include "xlate-jobs/TaskQueue.h"
#include <boost/asio.hpp>
#include <iosfwd>
#include <list>
#include <map>
#include <memory>
namespace xlate{

// forward decl
class TranslationJob;
class TranslationTask;

// class holding all jobs to be processed
class TranslationJobRepository{
public:
  // ctors, dtor
  TranslationJobRepository(boost::asio::io_service&ios,std::shared_ptr<JobQueue>qnew,std::shared_ptr<JobQueue>qsched,std::shared_ptr<TaskQueue>qtask);
  TranslationJobRepository(TranslationJobRepository const&)=delete;
  TranslationJobRepository(TranslationJobRepository&&)=default;
  TranslationJobRepository&operator=(TranslationJobRepository const&)=delete;
  TranslationJobRepository&operator=(TranslationJobRepository&&)=default;
  ~TranslationJobRepository()=default;

  // start listening on events
  void run();
private:
  // asio objects
  boost::asio::io_service&ios_;
  std::shared_ptr<JobQueueListener>qnewListener_;     // new jobs
  std::shared_ptr<JobQueueSender>qschedSender_;       // jobs to be scheduled
  std::shared_ptr<TaskQueueListener>qtaskListener_;   // translated tasks

  // list of new jobs and map of jobs in progress
  std::list<std::shared_ptr<TranslationJob>>newJobs_;
  std::map<TranslationJobId,std::shared_ptr<TranslationJob>>schedJobs_;

  // track if scheduler queue is blocked
  bool waiting4unblock_;

  // helper functions
  void waitForUnblock();
  void waitForNewJob();
  void waitForTranslatedTask();

  // helper functions
  void waitUnblockHandler(boost::system::error_code const&ec);
  void newJobHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationJob>job);
  void translatedTaskHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task);

  // some stats
  std::size_t ndone_{0};
};
}
#endif

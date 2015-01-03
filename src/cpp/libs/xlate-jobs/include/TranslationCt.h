// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#ifndef __TRANSLATION_CT_H__
#define __TRANSLATION_CT_H__
#include "xlate-jobs/JobQueue.h"
#include "xlate-jobs/TaskQueue.h"
#include <boost/asio.hpp>
#include <cstddef>
#include <memory>
#include <vector>
#include <ratio>
namespace xlate{

// forward decl
class TranslationJobRepository;
class TaskScheduler;
class EngineProxy;
class EngineEnv;

// translation component for a language pair
class TranslationCt{
public:
  // ctors,assign,dtor
  TranslationCt(boost::asio::io_service&ios,std::size_t maxScheduledJobs,std::size_t maxEngines,std::shared_ptr<EngineEnv>enngineenv);
  TranslationCt(TranslationCt const&);
  TranslationCt(TranslationCt&&);
  TranslationCt&operator=(TranslationCt const&)=delete;
  TranslationCt&operator=(TranslationCt&&)=default;
  ~TranslationCt();

  // start translation componnet
  void run();
  void stop();

  // get #of jobs in system
  std::size_t size()const;

  // queue getters
  std::shared_ptr<JobQueue>getNewJobQueue()const;
  std::shared_ptr<JobQueue>getTranslatedJobQueue()const;
private:
  // asio service
  boost::asio::io_service&ios_;

  // other configuration parameters
  std::size_t maxEngines_;
  std::shared_ptr<EngineEnv>enngineenv_;

  // queue configuration parameters
  std::size_t qnewJobSize_=std::mega::num;                   // new jobs into repository - can be very large
  std::size_t qschedTaskSize_;                               // queue size from scheduler to engines (should be same size as #of engines)
  std::size_t qschedJobSize_;                                // queue size between job repository and scheduler
  std::size_t qtransTasksSize_=std::mega::num;               // translated queue size - can be very large
  std::size_t qtransJobSize_=std::mega::num;                 // translated job size - can be very large

  // queues in the system
  std::shared_ptr<JobQueue>qnewJob_;
  std::shared_ptr<JobQueue>qschedJob_;
  std::shared_ptr<TaskQueue>qschedTask_;
  std::shared_ptr<TaskQueue>qtransTasks_;
  std::shared_ptr<JobQueue>qtransJobs_;

  // components
  std::shared_ptr<TranslationJobRepository>jobrep_;
  std::shared_ptr<TaskScheduler>scheduler_;
  std::vector<std::shared_ptr<EngineProxy>>engines_;
};
}
#endif

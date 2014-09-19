#ifndef __ENGINE_PROXY_H__
#define __ENGINE_PROXY_H__
#include "xlate-jobs/TaskQueue.h"
#include <memory>
namespace xlate{

// forward decl.
class TranslationTask;

// class scheduling tasks taken from a job repository
class EngineProxy{
public:
  EngineProxy(boost::asio::io_service&ios,std::shared_ptr<TaskQueue>qin,std::shared_ptr<TaskQueue>qout);
  EngineProxy(EngineProxy const&)=delete;
  EngineProxy(EngineProxy&&)=default;
  EngineProxy&operator=(EngineProxy const&)=delete;
  EngineProxy&operator=(EngineProxy&&)=default;
  ~EngineProxy()=default;

  // run engine
  void run();

  // getters
  EngineProxyId id()const;
private:
  // helper functions
  void waitForNewTask();
  void newTaskHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task);
  void tmoHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task);

  // asio stuff
  boost::asio::io_service&ios_;
  std::shared_ptr<TaskQueueListener>qtaskListener_;
  std::shared_ptr<TaskQueueSender>qtaskSender_;
  EngineProxyId id_;
  boost::asio::deadline_timer tmo_;
};
}
#endif

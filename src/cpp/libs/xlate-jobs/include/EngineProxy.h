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
  EngineProxy(std::shared_ptr<TaskQueue>qin,std::shared_ptr<TaskQueue>qout);
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
  std::shared_ptr<TaskQueue>qin_;
  std::shared_ptr<TaskQueue>qout_;
  EngineProxyId id_;

  // transate a task
  void translate(std::shared_ptr<TranslationTask>)const;
};
}
#endif

#ifndef __ENGINE_ADAPTOR_H__
#define __ENGINE_ADAPTOR_H__
#include "xlate-jobs/TaskQueue.h"
#include <memory>
namespace xlate{

// forward decl.
class TranslationTask;

// class scheduling tasks taken from a job repository
class EngineAdaptor{
public:
  EngineAdaptor(std::shared_ptr<TaskQueue>qin,std::shared_ptr<TaskQueue>qout);
  EngineAdaptor(EngineAdaptor const&)=delete;
  EngineAdaptor(EngineAdaptor&&)=default;
  EngineAdaptor&operator=(EngineAdaptor const&)=delete;
  EngineAdaptor&operator=(EngineAdaptor&&)=default;
  ~EngineAdaptor()=default;

  // run engine
  void run();

  // getters
  EngineAdaptorId id()const;
private:
  std::shared_ptr<TaskQueue>qin_;
  std::shared_ptr<TaskQueue>qout_;
  EngineAdaptorId id_;

  // transate a task
  void translate(std::shared_ptr<TranslationTask>)const;
};
}
#endif

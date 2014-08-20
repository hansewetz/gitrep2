#ifndef __DUMMY_ENGINE_H__
#define __DUMMY_ENGINE_H__
#include "xlate-jobs/TaskQueue.h"
#include <memory>
namespace xlate{

// forward decl.
class TranslationTask;

// class scheduling tasks taken from a job repository
class DummyEngine{
public:
  DummyEngine(std::shared_ptr<TaskQueue>qin,std::shared_ptr<TaskQueue>qout);
  DummyEngine(DummyEngine const&)=delete;
  DummyEngine(DummyEngine&&)=default;
  DummyEngine&operator=(DummyEngine const&)=delete;
  DummyEngine&operator=(DummyEngine&&)=default;
  ~DummyEngine()=default;

  // run engine
  void run();

  // getters
  DummyEngineId id()const;
private:
  std::shared_ptr<TaskQueue>qin_;
  std::shared_ptr<TaskQueue>qout_;
  DummyEngineId id_;
};
}
#endif

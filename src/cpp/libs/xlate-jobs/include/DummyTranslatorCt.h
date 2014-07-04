#ifndef __DUMMY_TRANSLATOR_CT_H__
#define __DUMMY_TRANSLATOR_CT_H__
#include <iosfwd>
#include <memory>
namespace xlate{

// forward decl
class TaskQueue;
class TranslationJobRepository;
class TaskScheduler;
class TaskCollector;
class DummyEngine;

// translation component for a single language pair
class DummyTranslatorCt{
public:
  // ctors
  DummyTranslatorCt(std::shared_ptr<TranslationJobRepository>jobrep);
  DummyTranslatorCt(DummyTranslatorCt const&)=delete;
  DummyTranslatorCt(DummyTranslatorCt&&)=delete;
  DummyTranslatorCt&operator=(DummyTranslatorCt const&)=default;
  DummyTranslatorCt&operator=(DummyTranslatorCt&&)=default;
  ~DummyTranslatorCt()=default;

  // run and terminate method
  void operator()();
  void terminate();
private:
  // Notice: do not modify the order of these attributes
  std::shared_ptr<TranslationJobRepository>jobrep_;
  std::shared_ptr<TaskQueue>qin_;
  std::shared_ptr<TaskQueue>qout_;
  std::shared_ptr<TaskScheduler>scheduler_;
  std::shared_ptr<TaskCollector>collector_;
  std::shared_ptr<DummyEngine>engine_;
};
}
#endif

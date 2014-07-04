#ifndef __DUMMY_TRANSLATION_ENGINE_H__
#define __DUMMY_TRANSLATION_ENGINE_H__
#include <iosfwd>
#include <memory>
using namespace std;
namespace xlate{

// forward decl
class TaskQueue;

// dummy translator - echo what it is given
class DummyTranslationEngine{
public:
  DummyTranslationEngine(std::shared_ptr<TaskQueue>inq,std::shared_ptr<TaskQueue>outq);
  DummyTranslationEngine(DummyTranslationEngine const&)=delete;
  DummyTranslationEngine(DummyTranslationEngine&&)=default;
  DummyTranslationEngine&operator=(DummyTranslationEngine const&)=delete;
  DummyTranslationEngine&operator=(DummyTranslationEngine&&)=default;
  ~DummyTranslationEngine()=default;

  // run task collector (moves tasks from task queue into job repository)
  void operator()();

  // debug print operator
  friend std::ostream&operator<<(std::ostream&os,DummyTranslationEngine const&dt);
private:
  std::shared_ptr<TaskQueue>inq_;
  std::shared_ptr<TaskQueue>outq_;
};
}
#endif

#ifndef __DUMMY_TRANSLATOR_H__
#define __DUMMY_TRANSLATOR_H__
#include <iosfwd>
#include <memory>
using namespace std;
namespace xlate{

// forward decl
class TaskQueue;

// dummy translator - echo what it is given
class DummyTranslator{
public:
  DummyTranslator(std::shared_ptr<TaskQueue>inq,std::shared_ptr<TaskQueue>outq);
  DummyTranslator(DummyTranslator const&)=delete;
  DummyTranslator(DummyTranslator&&)=default;
  DummyTranslator&operator=(DummyTranslator const&)=delete;
  DummyTranslator&operator=(DummyTranslator&&)=default;
  ~DummyTranslator()=default;

  // run task collector (moves tasks from task queue into job repository)
  void operator()();

  // debug print operator
  friend std::ostream&operator<<(std::ostream&os,DummyTranslator const&dt);
private:
  std::shared_ptr<TaskQueue>inq_;
  std::shared_ptr<TaskQueue>outq_;
};
}
#endif

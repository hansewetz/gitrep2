#ifndef __DUMMY_ENGINE_H__
#define __DUMMY_ENGINE_H__
#include <iosfwd>
#include <memory>
using namespace std;
namespace xlate{

// forward decl
class TaskQueue;

// dummy translator - echo what it is given
class DummyEngine{
public:
  DummyEngine(std::shared_ptr<TaskQueue>inq,std::shared_ptr<TaskQueue>outq);
  DummyEngine(DummyEngine const&)=delete;
  DummyEngine(DummyEngine&&)=default;
  DummyEngine&operator=(DummyEngine const&)=delete;
  DummyEngine&operator=(DummyEngine&&)=default;
  ~DummyEngine()=default;

  // run task collector (moves tasks from task queue into job repository)
  void operator()();

  // debug print operator
  friend std::ostream&operator<<(std::ostream&os,DummyEngine const&dt);
private:
  std::shared_ptr<TaskQueue>inq_;
  std::shared_ptr<TaskQueue>outq_;
};
}
#endif

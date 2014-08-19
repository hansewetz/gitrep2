#ifndef __DUMMY_ENGINE_H__
#define __DUMMY_ENGINE_H__
#include "xlate-jobs/TaskQueue.h"
#include <memory>
#include <boost/asio.hpp>
namespace xlate{

// forward decl.
class TranslationTask;

// class scheduling tasks taken from a job repository
class DummyEngine{
public:
  DummyEngine(boost::asio::io_service&ios,std::shared_ptr<TaskQueue>qin,std::shared_ptr<TaskQueue>qout);
  DummyEngine(DummyEngine const&)=delete;
  DummyEngine(DummyEngine&&)=default;
  DummyEngine&operator=(DummyEngine const&)=delete;
  DummyEngine&operator=(DummyEngine&&)=default;
  ~DummyEngine()=default;

private:
  // asio stuff
  boost::asio::io_service&ios_;
  std::shared_ptr<TaskQueueListener>qlistener_;
  std::shared_ptr<TaskQueueSender>qsender_;

  // helper functions
  void waitForNewTask();
  void taskHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task);
};
}
#endif

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#ifndef __ENGINE_PROXY_H__
#define __ENGINE_PROXY_H__
#include "asio-extensions/asio_queue.hpp"
#include "xlate-jobs/TaskQueue.h"
#include <memory>
#include <functional>
namespace xlate{

// forward decl.
class TranslationTask;
class EngineEnv;

// class scheduling tasks taken from a job repository
class EngineProxy{
public:
  // state of this engine proxy
  enum class state_t{NOT_RUNNING=0,WAITING4TASK=1,TRANSLATING=2,WAITING2TRANSLATE=3};

  // typedefs for queue types talking to engine
  using qToEngine_t=boost::asio::fdenq_queue<std::string,std::function<void(std::ostream&,std::string const&)>>;
  using qFromEngine_t=boost::asio::fddeq_queue<std::string,std::function<std::string(std::istream&)>>;

  // ctors,assign,dtor
  EngineProxy(boost::asio::io_service&ios,std::shared_ptr<TaskQueueDeq>qin,std::shared_ptr<TaskQueueEnq>qout,std::shared_ptr<EngineEnv>engineenv);
  EngineProxy(EngineProxy const&)=delete;
  EngineProxy(EngineProxy&&)=default;
  EngineProxy&operator=(EngineProxy const&)=delete;
  EngineProxy&operator=(EngineProxy&&)=default;
  ~EngineProxy();

  // run engine
  void run();
  void stop();

  // getters
  EngineProxyId id()const;
private:
  // asio handler functions
  void waitForNewTask();
  void newTaskHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task);
  void engineListenerHandler(boost::system::error_code const&ec,std::string const&msg,std::shared_ptr<TranslationTask>task);
  void waitToTranslateHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task);

  // helper to stopo an engine
  void stopEngine();

  // id of this engine
  EngineProxyId id_;

  // asio stuff
  boost::asio::io_service&ios_;

  // queues taking in tasks and sending translated tasks
  std::shared_ptr<TaskQueueListener>qtaskListener_;
  std::shared_ptr<TaskQueueSender>qtaskSender_;

  // state of proxy
  state_t state_;

  // pid of engine child
  int cpid_;

  // engine environment
  std::shared_ptr<EngineEnv>engineenv_;

  // #of outstanding segments (i.e., we are have timed out on them)
  std::size_t ntmos_;

  // timeout (changes depdening on if engine has started or not
  std::size_t realTmoMs_;

  // queues talking to engines
  std::shared_ptr<qToEngine_t>qToEngine_;
  std::shared_ptr<qFromEngine_t>qFromEngine_;

  // sender/listener talking to engines
  std::shared_ptr<boost::asio::queue_sender<qToEngine_t>>qsenderToEngine_;
  std::shared_ptr<boost::asio::queue_listener<qFromEngine_t>>qListenerFromEngine_;
};
}
#endif

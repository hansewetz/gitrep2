#ifndef __ENGINE_PROXY_H__
#define __ENGINE_PROXY_H__
#include "asio-extensions/asio_queue.hpp"
#include "xlate-jobs/TaskQueue.h"
#include <memory>
#include <functional>
namespace xlate{

// forward decl.
class TranslationTask;

// class scheduling tasks taken from a job repository
class EngineProxy{
public:
  // NOTE! program to start - hard coded it for right now
  constexpr static char const*PROGDIR="/ec/prod/exodus/dgt/local/exodus/user/potocva/OPERATINGHOME/ensv-all";
  constexpr static char const*PROGPATH="/ec/prod/exodus/dgt/local/exodus/user/potocva/OPERATINGHOME/ensv-all/junk";
  constexpr static char const*PROGNAME="translate";

  // state of this engine proxy
  enum class state_t{NOT_RUNNING=0,WAITING4TASK=1,TRANSLATING=2};

  // typedefs for queue types talking to engine
  using qToEngine_t=boost::asio::fdenq_queue<std::string,std::function<void(std::ostream&,std::string const&)>>;
  using qFromEngine_t=boost::asio::fddeq_queue<std::string,std::function<std::string(std::istream&)>>;

  // ctors,assign,dtor
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
  // asio handler functions
  void waitForNewTask();
  void newTaskHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task);
  void engineListenerHandler(boost::system::error_code const&ec,std::string const&msg,std::shared_ptr<TranslationTask>task);

  // id of this engine
  EngineProxyId id_;

  // state of proxy
  // NOTE! We don;t really use state for anything ... we can probably get rid of it
  state_t state_;

  // translation tmo ms
  std::size_t xlateTmoMs_=3000; // NOTE! Hard coded

  // asio stuff
  boost::asio::io_service&ios_;

  // queues taking in tasks and sending translated tasks
  std::shared_ptr<TaskQueueListener>qtaskListener_;
  std::shared_ptr<TaskQueueSender>qtaskSender_;

  // queues talking to engines
  std::shared_ptr<qToEngine_t>qToEngine_;
  std::shared_ptr<qFromEngine_t>qFromEngine_;

  // sender/listener talking to engines
  std::shared_ptr<boost::asio::queue_sender<qToEngine_t>>qsenderToEngine_;
  std::shared_ptr<boost::asio::queue_listener<qFromEngine_t>>qListenerFromEngine_;
};
}
#endif

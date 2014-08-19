#include "xlate-jobs/DummyEngine.h"
#include "xlate-jobs/TranslationTask.h"
#include <boost/log/trivial.hpp>
#include <functional>
using namespace std;
using namespace std::placeholders;
namespace xlate{

// ctor
DummyEngine::DummyEngine(boost::asio::io_service&ios,std::shared_ptr<TaskQueue>qin,std::shared_ptr<TaskQueue>qout):
    ios_(ios),qlistener_{make_shared<TaskQueueListener>(ios,qin)},qsender_{make_shared<TaskQueueSender>(ios,qout)}{

  // start listening on events
  waitForNewTask();
}
// enable wait for new task
void DummyEngine::waitForNewTask(){
  BOOST_LOG_TRIVIAL(debug)<<"DummyEngine::waitForNewTask - enabling receiving task";
  qlistener_->async_deq(std::bind(&DummyEngine::taskHandler,this,_1,_2));
}
// handle a received task
void DummyEngine::taskHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task){
  BOOST_LOG_TRIVIAL(debug)<<"DummyEngine::taskHandler - received task: "<<*task;

  // NOTE! Should do some processing here
  task->setTargetSeg(task->srcSeg()+" - xlate");

  // send translated segment
  qsender_->async_enq(task,[](boost::system::error_code const&ec){}); // NOTE! Should be a synchronous send

  // wait for next task
  waitForNewTask();
}
}

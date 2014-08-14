#include "xlate-jobs/TaskScheduler.h"
#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TranslationTask.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TaskQueue.h"
#include "utils/utility.h"
#include <boost/log/trivial.hpp>
#include <iostream>
using namespace std;
using namespace std::placeholders;

namespace xlate{
// ctor
TaskScheduler::TaskScheduler(boost::asio::io_service&ios,shared_ptr<TranslationJobRepository>jobrep,std::shared_ptr<TaskQueue>q):
    ios_(ios),jobrep_(jobrep),q_(q),qsender_(ios,q){

  // do an async wait until we can send on queue
  qsender_.async_wait_enq(std::bind(&TaskScheduler::waitSend,this,_1));
}
// handler for OK to send on queue
void TaskScheduler::waitSend(boost::system::error_code const&ec){
  BOOST_LOG_TRIVIAL(debug)<<"got wait in scheduler";

  // NOTE! Must get job from job repository and start sending + setup wait again
  //       Must keep track of if we have a job 
  //       Setup state machine for scheduler

}
// debug print operator
ostream&operator<<(ostream&os,TaskScheduler const&tc){
  return os<<"#waiting tasks: "<<tc.q_->size();
}
}

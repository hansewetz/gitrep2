#include "asio-extensions/queue_listener.h"
#include "asio-extensions/queue_sender.h"
#include "asio-extensions/simple_queue.h"

#include "xlate-jobs/TaskQueue.h"
#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/TranslationTask.h"

#include <boost/asio.hpp> 
#include <boost/log/trivial.hpp>
#include <iostream>
#include <string>
#include <ratio>
#include <memory>
#include <vector>

using namespace std;
using namespace std::placeholders;
using namespace xlate;

// --------------- setup and populate a job repository
std::shared_ptr<TranslationJobRepository>getJobRepos(){
  LanguagePair lanp{make_lanpair("en","sv")};
  vector<string>segs{"Hello World","Goodbuy World","Last message","24 hours", "many movies"};
  std::shared_ptr<TranslateRequest>req{std::make_shared<TranslateRequest>(lanp,segs)};
  std::shared_ptr<TranslationJob>job{std::make_shared<TranslationJob>(req)};
  std::shared_ptr<TranslationJobRepository>jobrep=make_shared<TranslationJobRepository>(lanp);
  jobrep->addJob(job);
  return jobrep;
}

// --------------- setup asio stuff
boost::asio::io_service ios;

// --------------- setup request queue stuff
size_t max_outstanding_req{30};
std::shared_ptr<TaskQueue>qreq{make_shared<TaskQueue>(max_outstanding_req)};
boost::asio::simple_queue_sender<std::shared_ptr<TranslationTask>>qreq_sender(::ios,qreq);
boost::asio::simple_queue_listener<std::shared_ptr<TranslationTask>>qreq_listener(::ios,qreq);

//  -------------- handler for receiving requests
void req_receive(boost::system::error_code const&ec,std::shared_ptr<TranslationTask>task){
  if(ec)BOOST_LOG_TRIVIAL(debug)<<"got error: "<<ec;
  else{
    BOOST_LOG_TRIVIAL(debug)<<"got task: "<<*task;
    qreq_listener.async_deq(req_receive);
  }
}
// main test program
int main(){
  try{
    // get a populated job repository
    std::shared_ptr<TranslationJobRepository>jobrep1{getJobRepos()};

    // get next job from repository and send translation tasks 
    std::shared_ptr<TranslationJob>job{jobrep1->getNextJob()};
    std::shared_ptr<TranslationTask>task;
    while(task=job->getNextTask()){
      qreq_sender.async_enq(task,[](boost::system::error_code const&ec){});
    }
    // test receiving a message
    qreq_listener.async_deq(req_receive);

    // run io loop
    ::ios.run();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}

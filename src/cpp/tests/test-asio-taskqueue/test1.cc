#include "xlate-jobs/JobQueue.h"
#include "xlate-jobs/TaskQueue.h"
#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJob.h"

#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TaskScheduler.h"
#include "xlate-jobs/DummyEngine.h"

#include "utils/logUtils.h"

#include <boost/asio.hpp> 
#include <boost/log/trivial.hpp>

#include <iostream>
#include <string>
#include <ratio>
#include <memory>
#include <vector>
#include <thread>

using namespace std;
using namespace std::placeholders;
using namespace xlate;

// --------------- setup asio stuff
boost::asio::io_service ios;

// --------------- setup job queues
std::shared_ptr<JobQueue>qnewJob{make_shared<JobQueue>(1000)};
std::shared_ptr<JobQueue>qschedJob{make_shared<JobQueue>(1)};
std::shared_ptr<TaskQueue>qschedTask{make_shared<TaskQueue>(3)};
std::shared_ptr<TaskQueue>qtransTasks{make_shared<TaskQueue>(1000)};

// some constants
constexpr size_t maxengines{10};

// --------------- create a job
std::shared_ptr<TranslationJob>getNextJob(){
  // create job request
LanguagePair lanp{make_lanpair("en","sv")};
  vector<string>segs{"Hello World","Goodby World","Last message","24 hours", "many movies"};
  std::shared_ptr<TranslateRequest>req{std::make_shared<TranslateRequest>(lanp,segs)};

  // create job from request
  return std::make_shared<TranslationJob>(req);
}
//  -------------- main test program
int main(){
  // set log level (do not log debug messages)
  utils::initBoostFileLogging(false);
  try{
    // ----------------- create structure 
    // create job repository, scheduler and start them
    std::shared_ptr<TranslationJobRepository>jobrep{make_shared<TranslationJobRepository>(::ios,qnewJob,qschedJob,qtransTasks)};
    std::shared_ptr<TaskScheduler>scheduler{make_shared<TaskScheduler>(::ios,qschedJob,qschedTask)};
    jobrep->run();
    scheduler->run();

    // create engines and start running each engine in a separate thread
    vector<std::thread>thr_engines;
    vector<std::shared_ptr<DummyEngine>>engines;
    for(int i=0;i<maxengines;++i){
      std::shared_ptr<DummyEngine>engine{make_shared<DummyEngine>(qschedTask,qtransTasks)};
      engines.push_back(engine);
      std::thread thr(&DummyEngine::run,engine);
      thr_engines.push_back(std::move(thr));
    }
    // ----------------- setup test
    // test: create job and send it on new job queue
    std::shared_ptr<JobQueueSender>qnewjobSender{make_shared<JobQueueSender>(::ios,qnewJob)};
    for(int i=0;i<2;++i){
      std::shared_ptr<TranslationJob>job{getNextJob()};
      qnewjobSender->async_enq(job,[](boost::system::error_code const&ec){});
    }
    // ----------------- run test
    ::ios.run();

    // join engine threads
    for(auto&thr:thr_engines)thr.join();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}

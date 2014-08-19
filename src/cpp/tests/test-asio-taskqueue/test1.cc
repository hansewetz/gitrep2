#include "xlate-jobs/JobQueue.h"
#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJobRepository.h"
#include "xlate-jobs/TranslationJob.h"

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

// --------------- setup asio stuff
boost::asio::io_service ios;

// --------------- setup job queues
std::shared_ptr<JobQueue>qnewJob{make_shared<JobQueue>(1000)};
std::shared_ptr<JobQueue>qschedJob{make_shared<JobQueue>(1)};

// language pair we are testing
LanguagePair lanp{make_lanpair("en","sv")};

// --------------- create a job
std::shared_ptr<TranslationJob>getNextJob(){
  // create job request
  vector<string>segs{"Hello World","Goodby World","Last message","24 hours", "many movies"};
  std::shared_ptr<TranslateRequest>req{std::make_shared<TranslateRequest>(lanp,segs)};

  // create job from request
  return std::make_shared<TranslationJob>(req);
}
// listen on scheduler queue
std::shared_ptr<JobQueueListener>schedListener;
void schedQueueListener(boost::system::error_code const&ec,std::shared_ptr<TranslationJob>job){
  BOOST_LOG_TRIVIAL(debug)<<"Scheduler queue: "<<*job;
  schedListener->async_deq(schedQueueListener);
}
// main test program
int main(){
  try{
    // create job repository
    std::shared_ptr<TranslationJobRepository>jobrep=make_shared<TranslationJobRepository>(::ios,qnewJob,qschedJob,lanp);

    // create job and send it on new job queue
    std::shared_ptr<TranslationJob>job{getNextJob()};
    std::shared_ptr<JobQueueSender>qnewjobSender{make_shared<JobQueueSender>(::ios,qnewJob)};
    qnewjobSender->async_enq(job,[](boost::system::error_code const&ec){});

    // create listener on scheduler queue
    schedListener=make_shared<JobQueueListener>(::ios,qschedJob);
//    schedListener->async_deq(schedQueueListener);

    // run io loop
    ::ios.run();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}

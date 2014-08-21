#include "xlate-jobs/JobQueue.h"
#include "xlate-jobs/TranslationCt.h"
#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/LanguageCode.h"
#include "xlate-tools/TranslationRequestFactory.h"
#include "utils/logUtils.h"

#include <boost/asio.hpp> 
#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <thread>

using namespace std;
using namespace std::placeholders;
using namespace xlate;

//  -------------- main test program
int main(){
  // setup asio stuff
  boost::asio::io_service io_service;

  // set log level (do not log debug messages)
  utils::initBoostFileLogging(true);
  try{
    // run translation component
    TranslationCt tct{io_service,1,1};
    tct.run();

    // create a request from a file and then a job
    boost::filesystem::path file{"./seg1.txt"};
    TranslationRequestFactory reqFact;
    std::shared_ptr<TranslateRequest>req{reqFact.requestFromSegmentedFile(make_lanpair("en","sv"),file)};
    std::shared_ptr<TranslationJob>job{make_shared<TranslationJob>(req)};

    // send job to translation
    std::shared_ptr<JobQueue>qjob{tct.getNewJobQueue()};
    std::shared_ptr<JobQueueSender>qsender{make_shared<JobQueueSender>(io_service,qjob)};
    qsender->async_enq(job,[](boost::system::error_code const&ec){});

    // run test
    io_service.run();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}

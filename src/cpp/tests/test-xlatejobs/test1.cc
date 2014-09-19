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
#include <thread>

using namespace std;
using namespace std::placeholders;
using namespace xlate;

// handler for jobs that have been translated
// (output queue from job repository)
void translatedJobHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationJob>job,std::shared_ptr<JobQueueListener>qtransjobreceiver){
  BOOST_LOG_TRIVIAL(info)<<"translated job: "<<*job;
  qtransjobreceiver->async_deq(std::bind(translatedJobHandler,_1,_2,qtransjobreceiver));
}
//  main test program
int main(){
  // setup asio stuff
  boost::asio::io_service ios;

  // set log level
  // (true: log debug info, false: do not log debug info)
  utils::initBoostFileLogging(false);
  try{
    // (1) ------------ create a translation component
    // (one translation component <--> one language pair)
    TranslationCt tct{ios,1,3};
    tct.run();

    // (2) ------------ create receiver of translated jobs
    // (we'll use the receiver to print out jobs which have been translated)
    std::shared_ptr<JobQueueListener>qtransjobreceiver{make_shared<JobQueueListener>(ios,tct.getTranslatedJobQueue())};
    qtransjobreceiver->async_deq(std::bind(translatedJobHandler,_1,_2,qtransjobreceiver));

    // (3) ------------ generate and send input for translation
    // create a request factory
    TranslationRequestFactory reqFact;

    // create sender to translation repository
    std::shared_ptr<JobQueueSender>qnewjobsender{make_shared<JobQueueSender>(ios,tct.getNewJobQueue())};

    // send jobs to translation component
    for(int i=0;i<5;++i){
      // create request from file
      boost::filesystem::path file{"./seg1.txt"};
      std::shared_ptr<TranslateRequest>req{reqFact.requestFromSegmentedFile(make_lanpair("en","sv"),file)};

      // create job from request and send it
      std::shared_ptr<TranslationJob>job{make_shared<TranslationJob>(req)};
      qnewjobsender->async_enq(job,[](boost::system::error_code const&ec){});
    }
    // (4) ------------ run asynchronous machinery
    // (everything runs under asio with the exception of the actual engines which runs as separate processes)
    BOOST_LOG_TRIVIAL(info)<<"starting asio ...";
    ios.run();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}

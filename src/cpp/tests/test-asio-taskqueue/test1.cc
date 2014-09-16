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

// translated job handler
void translatedJobHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationJob>job, std::shared_ptr<JobQueueListener>qtransjobreceiver){
  BOOST_LOG_TRIVIAL(info)<<"translated job: "<<*job;
  qtransjobreceiver->async_deq(std::bind(translatedJobHandler,_1,_2,qtransjobreceiver));
}
//  -------------- main test program
int main(){
  // setup asio stuff
  boost::asio::io_service io_service;

  // set log level (do not log debug messages)
  utils::initBoostFileLogging(false);
  try{
    // create and arm translation component
    TranslationCt tct{io_service,1,3};
    tct.run();

    // create a request factory
    TranslationRequestFactory reqFact;

    // create sender to translation repository
    std::shared_ptr<JobQueueSender>qnewjobsender{make_shared<JobQueueSender>(io_service,tct.getNewJobQueue())};

    // create receive of translated jobs and arm it
    std::shared_ptr<JobQueueListener>qtransjobreceiver{make_shared<JobQueueListener>(io_service,tct.getTranslatedJobQueue())};
    qtransjobreceiver->async_deq(std::bind(translatedJobHandler,_1,_2,qtransjobreceiver));

    // send jobs to translation component
    for(int i=0;i<5;++i){
      // create request from file
      boost::filesystem::path file{"./seg1.txt"};
      std::shared_ptr<TranslateRequest>req{reqFact.requestFromSegmentedFile(make_lanpair("en","sv"),file)};

      // create job from request and send it
      std::shared_ptr<TranslationJob>job{make_shared<TranslationJob>(req)};
      qnewjobsender->async_enq(job,[](boost::system::error_code const&ec){});
    }
    // run test
    io_service.run();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}

#include "xlate-jobs/TranslationCt.h"
#include "xlate-jobs/JobQueue.h"
#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJob.h"
#include "utils/logUtils.h"

#include <boost/asio.hpp> 
#include <boost/log/trivial.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <thread>

using namespace std;
using namespace std::placeholders;
using namespace xlate;

// setup asio stuff
boost::asio::io_service ios;

// testing: create a job
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
    // run translation component
    TranslationCt tct{::ios,3,10};
    tct.run();

    // run test
    ::ios.run();
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}

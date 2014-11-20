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
#include <boost/program_options.hpp>

#include <algorithm>
#include <iterator>
#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <thread>

using namespace std;
using namespace std::placeholders;
using namespace xlate;

namespace po=boost::program_options;
namespace fs=boost::filesystem;

namespace{
po::options_description options{string("usage: -h file ...")};
void usage(){
  std::cerr<<options;
  std::exit(1);
}
void usage(std::string const&msg){
  std::cerr<<msg<<endl;
  std::exit(1);
}
}
// global parameters
vector<string>files; // files to translate

// process command line params
void processCmdLineParams(int argc,char**argv){
  // add help option
  options.add_options()("help,h","help");
  options.add_options()("file,f","list of files to be translated");

  // --- setup for processing command line parametersprocess command line parameters
  po::variables_map vm;
  po::options_description cmdline_options("all command line options");

  // setup description of trailing (hidden) options
  po::options_description hidden_options("hidden options");
  hidden_options.add_options()("files",po::value<vector<string> >(),"list of files to be translated");
  po::positional_options_description p;
  p.add("files",-1);

  // consolidate all options into one description so we can parse command line
  cmdline_options.add(options).add(hidden_options);
  po::store(po::command_line_parser(argc,argv).options(cmdline_options).positional(p).run(),vm);

  // compile process options
  po::store(po::command_line_parser(argc,argv).options(cmdline_options).run(),vm);
  po::notify(vm);

  // get trailing options - files to late0if any)
  std::vector<std::string>trailing;
  if(vm.count("files")){
    trailing=vm["files"].as<vector<string>>();
  }
  // --- check for help
  if(vm.count("help"))usage();

  // get file to translate
  for(auto const&t:trailing){
    string const&file{t};
    if(!fs::exists(file))usage("string(file: )"+file+" is not a file or doe snot exist");
    files.push_back(file);
  }
  // make sure we have files to translate
  if(files.empty())usage("must specify at least one file to translate");
}
// handler for jobs that have been translated
// (output queue from job repository)
void translatedJobHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationJob>job,std::shared_ptr<JobQueueListener>qtransjobreceiver){
  BOOST_LOG_TRIVIAL(info)<<">>>>translated job: "<<*job;
  qtransjobreceiver->async_deq(std::bind(translatedJobHandler,_1,_2,qtransjobreceiver));
  
  // print translated segments
  list<shared_ptr<TranslationTask>>const&translated{job->translated()};
  for(shared_ptr<TranslationTask>task:translated)cout<<"["<<task->srcSeg()<<"]-->["<<task->targetSeg()<<"][jobid: "<<task->jobid()<<"][segno: "<<task->segno()<<"][id: "<<task->id()<<"]"<<endl;
  cout<<endl;
//  for(shared_ptr<TranslationTask>task:translated)cout<<"[jobid: "<<task->jobid()<<"]"<<endl;
}
//  main test program
int main(int argc,char**argv){
  // process command line parameters
  processCmdLineParams(argc,argv);

  // setup asio stuff
  boost::asio::io_service ios;

  // set log level
  // (true: log debug info, false: do not log debug info)
  utils::initBoostFileLogging(false);
  try{
    // (1) ------------ create a translation component
    // (one translation component <--> one language pair)
    size_t maxSegInParallel{3};
    size_t maxEngines{10};
    TranslationCt tct{ios,3,1};
    tct.run();

    // (2) ------------ create receiver of translated jobs
    // (we'll use the receiver to print out jobs which have been translated)
    std::shared_ptr<JobQueueListener>qtransjobreceiver{make_shared<JobQueueListener>(ios,tct.getTranslatedJobQueue().get())};
    qtransjobreceiver->async_deq(std::bind(translatedJobHandler,_1,_2,qtransjobreceiver));

    // (3) ------------ generate and send input for translation
    // create a request factory
    TranslationRequestFactory reqFact;

    // create sender to translation repository
    std::shared_ptr<JobQueueSender>qnewjobsender{make_shared<JobQueueSender>(ios,tct.getNewJobQueue().get())};

    // create request from file
    for(auto file:files){
      // create request, then job and send job for translation
      std::shared_ptr<TranslateRequest>req{reqFact.requestFromSegmentedFile(make_lanpair("en","sv"),file)};
      std::shared_ptr<TranslationJob>job{make_shared<TranslationJob>(req)};
      BOOST_LOG_TRIVIAL(info)<<"adding file: \""<<file<<"\"<< for translation (jobid: "<<job->id()<<")";
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

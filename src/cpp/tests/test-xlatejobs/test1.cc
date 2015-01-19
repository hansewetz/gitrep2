// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "xlate-jobs/JobQueue.h"
#include "xlate-jobs/TranslationCt.h"
#include "xlate-jobs/TranslateRequest.h"
#include "xlate-jobs/TranslationJob.h"
#include "xlate-jobs/LanguageCode.h"
#include "xlate-jobs/EngineEnv.h"
#include "xlate-tools/TranslationRequestFactory.h"
#include "xlate-tools/JobHandlerScreenPrinter.h"
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

// NOTE! Hard codeded - should be taken from command line parameters and having default
constexpr static char const*EXEDIR="/ec/prod/exodus/dgt/local/exodus/user/potocva/OPERATINGHOME/ensv-all";
constexpr static char const*PROGPATH="/ec/prod/exodus/dgt/local/exodus/user/potocva/OPERATINGHOME/ensv-all/translate";
constexpr static char const*PROGNAME="translate";
/*
constexpr static char const*EXEDIR="/bin";
constexpr static char const*PROGPATH="/bin/cat";
constexpr static char const*PROGNAME="cat";
*/
// namespace stuff
using namespace std;
using namespace std::placeholders;
using namespace xlate;

namespace po=boost::program_options;
namespace fs=boost::filesystem;

// setup asio stuff
boost::asio::io_service ios;

// usage information
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
// command line parameters
namespace{
vector<string>files;               // files to translate
size_t maxJobsInParallel{3};       // max #of jobs to translate in parallel
size_t maxEngines{1};              // max #of engines to start
size_t segTmoMs{10000};            // timeout in ms for a segment
size_t startTmoMs{120000};         // timeout in ms to start an engine
bool debug{false};                 // debuggging on/off
}
// print command line options
void prinCmdLineOptions(){
  cout<<"Command line options:"<<endl;
  cout<<"\tdebug: "<<boolalpha<<debug<<endl;
  cout<<"\t#engines: "<<maxEngines<<endl;
  cout<<"\tsegment tmo(ms): "<<segTmoMs<<endl;
  cout<<"\tengine startup time (ms): "<<startTmoMs<<endl;
  cout<<"\tfiles to translate:";
  for(auto const&f:files)cout<<' '<<f;
  cout<<endl;
}
// process command line params
void processCmdLineParams(int argc,char**argv){
  // add help option
  options.add_options()("help,h","help");
  options.add_options()("debug,d","debug turned on");
  options.add_options()("engines,e",po::value<size_t>(),(string("max #of engines (default ")+boost::lexical_cast<string>(maxEngines)+")").c_str());
  options.add_options()("tmo,t",po::value<size_t>(),(string("segment timeout in ms (default ")+boost::lexical_cast<string>(segTmoMs)+")").c_str());
  options.add_options()("tmostart,T",po::value<size_t>(),(string("timeout to start an engine (default ")+boost::lexical_cast<string>(startTmoMs)+")").c_str());
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
  // check for help
  if(vm.count("help"))usage();

  // check for debug
  if(vm.count("debug"))debug=true;

  // check for #of engines
  if(vm.count("engines")){
    if((maxEngines=vm["engines"].as<size_t>())==0)usage();
  }
  // check for segment timeout
  if(vm.count("tmo")){
    if((segTmoMs=vm["tmo"].as<size_t>())==0)usage();
  }
  // check for segment timeout
  if(vm.count("tmostart")){
    if((startTmoMs=vm["tmostart"].as<size_t>())==0)usage();
  }
  // get file to translate
  for(auto const&t:trailing){
    string const&file{t};
    if(!fs::exists(file))usage("string(file: )"+file+" is not a file or doe snot exist");
    files.push_back(file);
  }
  // make sure we have files to translate
  if(files.empty())usage("must specify at least one file to translate");
}
// handler for jobs that have been translated - also stops the system when no more jobs in repository
// (output queue from job repository)
void translatedJobHandler(boost::system::error_code const&ec,std::shared_ptr<TranslationJob>job,std::shared_ptr<JobQueueListener>qtransjobreceiver,TranslationCt*tct){
  // check error code
  if(ec==boost::asio::error::operation_aborted){
    BOOST_LOG_TRIVIAL(info)<<"::translatedJobHandler: operation interupted";
    return;
  }
  if(ec!=boost::system::error_code()){
    BOOST_LOG_TRIVIAL(info)<<"::translatedJobHandler: error, ec: "<<ec.message();
    return;
  }
  // print job to screen
  BOOST_LOG_TRIVIAL(info)<<">>>>translated job: "<<*job;
  shared_ptr<JobHandler>jobHandler=make_shared<JobHandlerScreenPrinter>(job);
  (*jobHandler)();

  // if we have processed all files, then stop asio
 if(tct->size()==0){
    // stop translation ct
    tct->stop();
  }else{
    // listen to next job
    qtransjobreceiver->async_deq(std::bind(translatedJobHandler,_1,_2,qtransjobreceiver,tct));
  }
}
//  main test program
int main(int argc,char**argv){
  // process command line parameters and print them on screen
  processCmdLineParams(argc,argv);
  prinCmdLineOptions();

  // set log level
  // (true: log debug info, false: do not log debug info)
  utils::initBoostFileLogging(debug);
  try{
    // (0) ------------ setup engine environment 
    shared_ptr<EngineEnv>engineenv=make_shared<EngineEnv>(EXEDIR,PROGPATH,PROGNAME,segTmoMs,startTmoMs);
    
    // (1) ------------ create a translation component and kick it off
    // (one translation component <--> one language pair)
    TranslationCt tct{::ios,maxJobsInParallel,maxEngines,engineenv};
    tct.run();

    // (2) ------------ create receiver of translated jobs
    // (we'll use the receiver to print out jobs which have been translated)
    std::shared_ptr<JobQueueListener>qtransjobreceiver{make_shared<JobQueueListener>(::ios,tct.getTranslatedJobQueue().get())};
    qtransjobreceiver->async_deq(std::bind(translatedJobHandler,_1,_2,qtransjobreceiver,&tct));

    // (3) ------------ generate and send input for translation
    // create a request factory
    TranslationRequestFactory reqFact;

    // (4) create sender to translation repository
    std::shared_ptr<JobQueueSender>qnewjobsender{make_shared<JobQueueSender>(::ios,tct.getNewJobQueue().get())};

    // (5) create request from files and send them to translation component
    for(auto const&file:files){
      // create request, then job and send job for translation
      std::shared_ptr<TranslateRequest>req{reqFact.requestFromSegmentedFile(make_lanpair("en","sv"),file)};
      std::shared_ptr<TranslationJob>job{make_shared<TranslationJob>(req)};
      BOOST_LOG_TRIVIAL(info)<<"adding file: \""<<file<<"\" for translation (jobid: "<<job->id()<<")";
      qnewjobsender->async_enq(job,[](boost::system::error_code const&ec){});
    }
    // (6) ------------ run asio machinery
    // (everything runs under asio with the exception of the actual engines which runs as separate processes)
    BOOST_LOG_TRIVIAL(info)<<"starting asio ...";
    ::ios.run();
    BOOST_LOG_TRIVIAL(info)<<"asio stopped";
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(debug)<<"cought exception: "<<e.what();
  }
}

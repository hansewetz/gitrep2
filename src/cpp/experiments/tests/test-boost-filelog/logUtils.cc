// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "logUtils.h"
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/attributes.hpp>
// namespace stuff
using namespace std;
namespace utils{

// initialize logging
void initBoostFileLogging(UtilsLogLevel level,string const&path,string const&basename,size_t maxsize,size_t maxfiles){
  // configure log level
  initBoostLogging(level);

  // get core for logging
  auto core=boost::log::core::get();
  
  // setup backend for text files
  // (rotated log files + maximum size of files + flush after each log message)
  using backend_t=boost::log::sinks::text_file_backend;
  boost::shared_ptr<backend_t>backend=boost::make_shared<backend_t>(
        boost::log::keywords::file_name=path+"/"+basename+"_%Y-%m-%d_%H:%M:%S_%5N.log",
        boost::log::keywords::rotation_size=maxsize
      );
  backend->auto_flush(true);

  // create a sink from backend
  using sink_t=boost::log::sinks::synchronous_sink<backend_t>;
  boost::shared_ptr<sink_t>sink(new sink_t(backend));

  // setup file collector
  // (we keep at most a total 'maxsize*maxfiles' bytes in log files --> keep 'maxfiles' logfiles)
  sink->locked_backend()->set_file_collector(boost::log::sinks::file::make_collector(
        boost::log::keywords::target=path,
        boost::log::keywords::max_size=maxsize*maxfiles)
  );
  // when restarting, scan the directory for files matching the file_name pattern
  // (this means that the numbering of a new log file will start at +1 from the numbering of the highest logfile in the 'path')
  sink->locked_backend()->scan_for_files();

  // add attributes
  core->add_global_attribute("TimeStamp",boost::log::attributes::local_clock());
  core->add_global_attribute("ThreadID",boost::log::attributes::current_thread_id());

  // setup formatting of log messages
  sink->set_formatter(
      boost::log::expressions::format("[%1%] [%2%] [%3%] %4%")%
      boost::log::expressions::attr<boost::posix_time::ptime>("TimeStamp")%
      boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID")%
      boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity")%
      boost::log::expressions::smessage
  );
  // add sink to core
  core->add_sink(sink);
}
// initialize logging to stdout
void initBoostLogging(UtilsLogLevel level){
  if(level==UtilsLogLevel::TRACE){
    boost::log::core::get()->set_filter(boost::log::trivial::severity>=boost::log::trivial::trace);
  }else
  if(level==UtilsLogLevel::DEBUG){
    boost::log::core::get()->set_filter(boost::log::trivial::severity>=boost::log::trivial::debug);
  }else{
    boost::log::core::get()->set_filter(boost::log::trivial::severity>=boost::log::trivial::info);
  }
}
// set debug/normal level
void initBoostLogging(bool debug){
  initBoostLogging(debug?UtilsLogLevel::DEBUG:UtilsLogLevel::NORMAL);
}
}

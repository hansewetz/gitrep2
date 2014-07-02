#include "utils/logUtils.h"
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/expressions/message.hpp>
#include <boost/log/expressions/formatter.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/attributes/current_thread_id.hpp>

// namespace stuff
using namespace std;
namespace logging=boost::log;
namespace utils{

// define a few keywords to be used in formatting
BOOST_LOG_ATTRIBUTE_KEYWORD(line_id,"LineID",unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity,"Severity",boost::log::trivial::severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp,"TimeStamp",boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(threadid,"ThreadID", boost::log::attributes::current_thread_id::value_type)

// initialize logging
void initBoostFileLogging(bool verbose,string const&path,string const&basename,size_t maxsize){
  // configure log level
  initBoostFileLogging(verbose);

  // add attributes to logger
  boost::log::add_common_attributes();
  auto core=boost::log::core::get();
  core->add_global_attribute("ThreadID",boost::log::attributes::current_thread_id());

  // configure writing to log file
  //(format: [2014-03-14 08:16:37.929047] [0x3d6b6b00] [info] message ...)
  boost::log::add_file_log(
    boost::log::keywords::file_name=path+"/"+basename+"_%Y-%m-%d_%H:%M:%S_%N.log",
    boost::log::keywords::rotation_size=maxsize,
    boost::log::keywords::format=(
      boost::log::expressions::stream << boost::log::expressions::format_date_time(timestamp,"[%Y-%m-%d %H:%M:%S] ")<<
      "["<<threadid<<"] "<<
      "["<<severity<<"] "<<
      boost::log::expressions::smessage
    )
  );
}
// initialize logging to stdout
void initBoostFileLogging(bool verbose){
  if(verbose){
    logging::core::get()->set_filter(logging::trivial::severity>=logging::trivial::debug);
  }else{
    logging::core::get()->set_filter(logging::trivial::severity>=logging::trivial::info);
  }
}
}

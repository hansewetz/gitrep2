// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

using namespace std;
namespace logging=boost::log;

// initialize logging
void initBoostFileLogging(bool verbose,string const&path,string const&basename,size_t maxsize){
  // check what level of logging we should set
  if(verbose){
    logging::core::get()->set_filter(logging::trivial::severity>=logging::trivial::debug);
  }else{
    logging::core::get()->set_filter(logging::trivial::severity>=logging::trivial::info);
  }
  // configure writing to log file
  boost::log::add_file_log(
    boost::log::keywords::file_name=path+"/"+basename+"_%Y-%m-%d_%H:%M:%S_%N.log",
    boost::log::keywords::rotation_size=maxsize);
}
int main(){
  initBoostFileLogging(false,".","test1",10);

  // try a few log statements
  BOOST_LOG_TRIVIAL(debug)<<"not visible";
  BOOST_LOG_TRIVIAL(info)<<"visible";
  BOOST_LOG_TRIVIAL(error)<<"visible";
}

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "soapxlateService.h"
#include "xlate.nsmap"
#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>
using namespace std;
namespace po=boost::program_options;

// program options and usage
namespace{
  po::options_description desc{string("usage: -p <port> -h")};
  void usage(){
    cerr<<desc;
    exit(1);
  }
}
// --- server
int main(int argc, char **argv){
  try{
    // parse command line
    desc.add_options()
      ("help,h","print help message")
      ("port,p",po::value<int>(),"server port");
    po::variables_map vm;
    po::store(po::parse_command_line(argc,argv,desc),vm);
    po::notify(vm);

    // check for help
    if(vm.count("help"))usage();

    // get port
    int port;
    if(vm.count("port")){
      if((port=vm["port"].as<int>())<=0)usage();
    }
    // get xlateulation service
    xlateService xlate;
    xlate.soap_stream_fault(std::cerr);
    
    // run service (listen for client connections)
    if (xlate.run(port)){
      xlate.soap_stream_fault(std::cerr);
      exit(1);
    }
  }
  catch(std::exception const&e){
    cerr<<"exception: "<<e.what()<<endl;
    exit(1);
  }
  return 0;
} 

//  --- methods
int xlateService::translate(double a, struct ns__asString&res){
  // write double into a stream
  stringstream sstream;
  sstream<<"\""<<a<<"\"";

  // duplicate string and set in return value
  char*s{strdup(sstream.str().c_str())};
  res.str=s;
  return SOAP_OK;
} 

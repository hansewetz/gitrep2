// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <string>
#include <vector>
#include <iostream>
#include <boost/process.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
using namespace std;
namespace bp=boost::process;
namespace bios=boost::iostreams;

// main test program
int main(){
  // create pipe between child and parent
  bp::pipe p{bp::create_pipe()};

  // run program
  // (must have sink close so that we'll terminate when reading from other end of pipe)
  {
    bios::file_descriptor_sink sink(p.sink,bios::close_handle);
    vector<string>prog{"/bin/ls","-l"};
    bp::child c{bp::execute(bp::initializers::set_args(prog),
                bp::initializers::bind_stdout(sink),
                bp::initializers::throw_on_error())};
  
    // wait for child to exit
    bp::wait_for_exit(c);
  }
  // read from other end of pipe
  bios::file_descriptor_source source(p.source,bios::close_handle);
  bios::stream<bios::file_descriptor_source>is{source};
  string line;
  while(getline(is,line))cout<<"MAIN: "<<line<<endl;
}

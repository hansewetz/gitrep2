// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include <string>
#include <vector>
#include <array>
#include <functional>
#include <iterator>
#include <iostream>
#include <boost/process.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/asio.hpp>
using namespace std;
using namespace std::placeholders;
namespace bp=boost::process;
namespace bios=boost::iostreams;
namespace asio=boost::asio;

// global buffer
asio::streambuf buf;

// main test program
int main(){
  // io_service object
  asio::io_service ios;

  // create pipe between child and parent
  bp::pipe p{bp::create_pipe()};

  // run program
  // (must make sure sink closes so that we'll terminate when reading from other end of pipe)
  bios::file_descriptor_source source(p.source,bios::close_handle);
  vector<string>prog{"/bin/cat"};
  bp::child c{bp::execute(bp::initializers::set_args(prog),
              bp::initializers::bind_stdin(source),
              bp::initializers::throw_on_error())};
  
  // write to child
  asio::posix::stream_descriptor aos(ios,p.sink);
  ostream os{&buf};
  os<<"Hello world"<<endl<<flush;
  asio::write(aos,buf);
  aos.cancel();
  aos.close();
  source.close();

  // wait for child to exit
  bp::wait_for_exit(c);
}

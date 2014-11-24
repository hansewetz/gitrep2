// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

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

// callback handler for read
void read_handler(boost::system::error_code const&err,size_t nbytes,asio::posix::stream_descriptor*ais){
  if(nbytes!=0){
    istreambuf_iterator<char>end;
    istreambuf_iterator<char>it{&buf};
    for(;it!=end;++it)cout<<*it;
    asio::async_read(*ais,buf,std::bind(read_handler,_1,_2,ais));
  }else{
    ais->cancel();
    ais->close();
  }
}
// main test program
int main(){
  // io_service object
  asio::io_service ios;

  // create pipe between child and parent
  bp::pipe p{bp::create_pipe()};

  // run program
  // (must make sure sink closes so that we'll terminate when reading from other end of pipe)
  bios::file_descriptor_sink sink(p.sink,bios::close_handle);
  vector<string>prog{"/bin/ls","-l",".."};
  bp::child c{bp::execute(bp::initializers::set_args(prog),
              bp::initializers::bind_stdout(sink),
              bp::initializers::throw_on_error())};
  sink.close();
  
  // read from other end of pipe asynchronously
  asio::posix::stream_descriptor ais(ios,p.source);
  asio::async_read(ais,buf,std::bind(read_handler,_1,_2,&ais));

  // run asio
  ios.run();

  // wait for child to exit
  bp::wait_for_exit(c);
}

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
  // setup callback function for async read
  asio::posix::stream_descriptor ais(ios,p.source);
  function<void(boost::system::error_code const&err,size_t nbytes)>f=[&](boost::system::error_code const&err,size_t nbytes){
      if(nbytes!=0){
        istreambuf_iterator<char>end;
        istreambuf_iterator<char>it{&buf};
        for(;it!=end;++it)cout<<*it;
        asio::async_read(ais,buf,f);
      };
    };
  // read from other end of pipe asynchronously
  asio::async_read(ais,buf,f);
  bios::file_descriptor_source source(p.source,bios::close_handle);

  // run asio
  ios.run();
}

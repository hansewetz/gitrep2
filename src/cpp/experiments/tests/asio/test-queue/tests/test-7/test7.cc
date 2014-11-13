/*
this program tests the 'fd_queue' in isolation
*/

#include <boost/asio_queue.hpp>
#include <boost/log/trivial.hpp>
#include <string>
#include <iostream>
#include <functional>
#include <memory>
#include <unistd.h>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

using namespace std;
namespace asio=boost::asio;
namespace io=boost::iostreams;

// get an istream from a file descriptor
std::shared_ptr<istream>makefdistream(int fd,bool close){
  return std::shared_ptr<istream>(
    new istream(new io::stream_buffer<io::file_descriptor_source>(fd,close?io::close_handle:io::never_close_handle)));
}
// get an ostream from a file descriptor
std::shared_ptr<ostream>makefdostream(int fd,bool close){
  return std::shared_ptr<ostream>(
    new ostream(new io::stream_buffer<io::file_descriptor_sink>(fd,close?io::close_handle:io::never_close_handle)));
}
// queue stuff
using qval_t=std::string;
std::function<void(ostream&,qval_t const&)>serialiser=[](ostream&os,qval_t const&s){os<<s;};
using enq_t=asio::fdenq_queue<qval_t,decltype(serialiser)>;

// test program
int main(){
  try{
    // create an fd-pipe with connected read/write fds
    int fd[2];
    pipe(fd);
    int fdread{fd[0]};
    int fdwrite{fd[1]};

    // create a an enq() side of a queue
    enq_t q{fdread,serialiser};
    
    // NOTE! Not yet done
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

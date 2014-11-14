/*
NOTE! How should we manage serialisation by using streams instead of file descriptors?
      It seems not possible to flush things to the other side unless we have an 'endl'
      Maybe we should read byte by byte and assemble messages ...

*/
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
std::shared_ptr<istream>makefd_istream(int fd,bool close){
  return std::shared_ptr<istream>(
    new istream(new io::stream_buffer<io::file_descriptor_source>(fd,close?io::close_handle:io::never_close_handle)));
}
// set fd to non-blocking
void setFdNonblock(int fd){
  int flags = fcntl(fd,F_GETFL,0);
  if(fcntl(fd, F_SETFL,flags|O_NONBLOCK)<0){
    std::string err{strerror(errno)};
    throw std::runtime_error(std::string("setFdNonblock: failed setting fd in non-blocking mode: ")+err);
  }
}
// queue type
using qval_t=string;

// serialize an object (char by char)
std::function<void(ostream&,qval_t const&)>serialiser=[](ostream&os,qval_t const&s){
  os<<s;
};
using enq_t=asio::fdenq_queue<qval_t,decltype(serialiser)>;

// test program
int main(){
  try{
    // create an fd-pipe with connected read/write fds
    int fd[2];
    if(pipe(fd)!=0){
      BOOST_LOG_TRIVIAL(error)<<"pipe failed";
      exit(1);
    }
    int fdread{fd[0]};
    int fdwrite{fd[1]};

    // create enq queue
    enq_t qin{fdwrite,serialiser};

    // enq() an item
    boost::system::error_code ec;
    qval_t msg{"Hello there again"};
    qin.enq(msg,ec);

    // read from other side of pipe
//    setFdNonblock(fdread);
    shared_ptr<istream>is{makefd_istream(fdread,false)};
    string line;
    while(getline(*is,line))cout<<line<<endl;
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

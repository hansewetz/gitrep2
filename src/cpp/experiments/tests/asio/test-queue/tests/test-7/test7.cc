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

// create an input-stream from an fdd
shared_ptr<istream>makeistream(int fd){
  return shared_ptr<istream>(new istream(new io::stream_buffer<io::file_descriptor_source>(fd,io::never_close_handle)));
}

// value type in queues
// (must work with operator<< and operator>>, and be default constructable)
using qval_t=std::string;

// setup serializer/de-serializer for queue + queue-type
std::function<void(ostream&,string const&)>serialiser=[](ostream&os,string const&s){os<<s;};
using queue_t=asio::fdenq_queue<qval_t,decltype(serialiser)>;

// test program
int main(){
  try{
    // create a queue
    int fd[2];
    pipe(fd);
    int fdread{fd[0]};
    int fdwrite{fd[1]};
    queue_t q{fdread,serialiser};
    
    // creta a stream so we can write easier to the fd
    shared_ptr<istream>is(makeistream(fdread));
    while(*is){
      cout<<*is<<endl;
    }
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

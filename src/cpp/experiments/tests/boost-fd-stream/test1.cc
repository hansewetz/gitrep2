#include <memory>
#include <ostream>
#include <istream>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

using namespace std;
namespace io=boost::iostreams;

// get an ostream from a file descriptor
std::shared_ptr<ostream>fdostream(int fd,bool close){
  return std::shared_ptr<ostream>(
    new ostream(new io::stream_buffer<io::file_descriptor_sink>(fd,close?io::close_handle:io::never_close_handle)));
}
// get an istream from a file descriptor
std::shared_ptr<istream>fdistream(int fd,bool close){
  return std::shared_ptr<istream>(
    new istream(new io::stream_buffer<io::file_descriptor_source>(fd,close?io::close_handle:io::never_close_handle)));
}
// main test program
int main(){
  std::shared_ptr<ostream>os{fdostream(1,false)};
  *os<<"Hello"<<endl;
}

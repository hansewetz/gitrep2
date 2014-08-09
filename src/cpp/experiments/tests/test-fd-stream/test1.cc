#include <iostream>
#include <memory>
#include <istream>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

namespace io=boost::iostreams;
using namespace std;

shared_ptr<istream>makeistream(int fd){
  return shared_ptr<istream>(new istream(new io::stream_buffer<io::file_descriptor_source>(fd,io::never_close_handle)));
}

// main.
int main(){
  shared_ptr<istream>is{makeistream(0)};
  string str;
  (*is)>>str;
  cout<<str<<endl;
}

#include "utils/FdAsyncLineReader.h"
#include "utils/utility.h"
#include <string>
#include <vector>
#include <functional>
#include <boost/asio.hpp>

using namespace std;
using namespace std::placeholders;
namespace asio=boost::asio;
namespace utils{

// ctor
FdAsyncLineReader::FdAsyncLineReader(asio::io_service&ios,int fd,size_t bufsize,function<void(string const&)>linecb):
    ios_(ios),fd_(fd),ais_(ios_,fd_),bufsize_(bufsize),buf_(bufsize_),linecb_(linecb){
  ais_.async_read_some(boost::asio::buffer(buf_,bufsize_),std::bind(&FdAsyncLineReader::read_handler,this,_1,_2));
}
// dtor
FdAsyncLineReader::~FdAsyncLineReader(){
  close();
}
// async read handler
void FdAsyncLineReader::read_handler(boost::system::error_code const&err,size_t nbytes){
  if(err==boost::asio::error::eof){
    close();
  }else
  if(err!=0){
    close();
    THROW_RUNTIME("FdAsyncLineReader::read_handler: error code: "<<err.value()<<", error: "<<err.message();)
  }else
  if(nbytes!=0){
    process_data(nbytes);
    ais_.async_read_some(boost::asio::buffer(buf_,bufsize_),std::bind(&FdAsyncLineReader::read_handler,this,_1,_2));
  }else{
    // unknown error
    close();
    THROW_RUNTIME("FdAsyncLineReader::read_handler: unknown error, read 0 bytes");
  }
}
// process read data
// (break it into lines and dump each line as we find them)
void FdAsyncLineReader::process_data(size_t nbytes){
  for(int i=0;i<nbytes;++i){
    char c{buf_[i]};
    if(c=='\n'){
      linecb_(line_);
      line_.clear();
    }else{
      line_.push_back(c);
    }
  }
}
// close connection
void FdAsyncLineReader::close(){
  if(!closed_){
    ais_.cancel();
    ais_.close();
    closed_=true;
  }
}
}

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "asio-utils/FdAsyncLineReader.h"
#include "utils/utility.h"
#include <string>
#include <vector>
#include <functional>
#include <boost/asio.hpp>

using namespace std;
using namespace std::placeholders;
namespace asio=boost::asio;
namespace asioutils{

// ctor
FdAsyncLineReader::FdAsyncLineReader(asio::io_service&ios,int fd,size_t bufsize,
  function<void(string const&)>linecb,std::function<void(boost::system::error_code const&)>errcb):
    ios_(ios),fd_(fd),ais_(ios_,fd_),bufsize_(bufsize),buf_(bufsize_),linecb_(linecb),errcb_(errcb){
  ais_.async_read_some(boost::asio::buffer(buf_,bufsize_),std::bind(&FdAsyncLineReader::read_handler,this,_1,_2));
}
// dtor
FdAsyncLineReader::~FdAsyncLineReader(){
  close();
}
// async read handler
void FdAsyncLineReader::read_handler(boost::system::error_code const&err,size_t nbytes){
  if(err!=0){
    close();
    errcb_(boost::system::error_code(err));
  }else
  if(nbytes!=0){
    process_data(nbytes);
    ais_.async_read_some(boost::asio::buffer(buf_,bufsize_),std::bind(&FdAsyncLineReader::read_handler,this,_1,_2));
  }else{
    // unknown error
    close();
    THROW_RUNTIME("FdAsyncLineReader::read_handler: unknown error, read 0 bytes, should never happen");
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

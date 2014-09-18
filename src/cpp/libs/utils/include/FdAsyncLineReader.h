#ifndef __FD_ASYNC_LINE_READER_H__
#define __FD_ASYNC_LINE_READER_H__
#include <string>
#include <vector>
#include <functional>
#include <boost/asio.hpp>
namespace utils{

// read asynchrounosly from fd and invoke a callback function for each read line
// (the callback function is called with a string which have had the newline ('\n') stripped)
class FdAsyncLineReader{
public:
  // ctor,assign,dtor
  FdAsyncLineReader(boost::asio::io_service&ios,int fd,std::size_t bufsize,std::function<void(std::string const&)>linecb);
  FdAsyncLineReader(FdAsyncLineReader const&)=delete;
  FdAsyncLineReader(FdAsyncLineReader&&)=default;
  FdAsyncLineReader&operator=(FdAsyncLineReader const&)=delete;
  FdAsyncLineReader&operator=(FdAsyncLineReader&&)=default;
  ~FdAsyncLineReader();
private:
  // private helper functions
  void read_handler(boost::system::error_code const&err,std::size_t nbytes);
  void process_data(std::size_t nbytes);
  void close();

  // private data
  boost::asio::io_service&ios_;
  int fd_;
  boost::asio::posix::stream_descriptor ais_;
  std::size_t bufsize_;
  std::vector<char>buf_;
  std::function<void(std::string const&)>linecb_;
  std::string line_;
  bool closed_=false;
};
}
#endif
// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <unistd.h>
#include <boost/asio.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <array>
using namespace std;

// io service
boost::asio::io_service ios;

// buffer to read into
std::array<char,5>buf;

// read handler
void handleRead(boost::system::error_code const&err,size_t nread,boost::asio::posix::stream_descriptor*in){
  cout<<"nread: "<<nread<<endl;
  for(int i=0;i<nread;++i)cout<<buf[i];

  // start reading async from stdin
  in->async_read_some(boost::asio::buffer(buf),
    boost::bind(handleRead,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,in));
}
// main.
int main(int argc,char**argv){
  // setup IO object
  boost::asio::posix::stream_descriptor in(::ios,::dup(STDIN_FILENO));

  // start reading async from stdin
  in.async_read_some(boost::asio::buffer(buf),
    boost::bind(handleRead,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,&in));

  // run async service
  ::ios.run();
}


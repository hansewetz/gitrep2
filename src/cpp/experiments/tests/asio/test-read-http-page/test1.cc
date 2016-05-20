// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <string>
#include <iostream>

// main.
int main(int argc,char**argv){
  // setup socket stream
  boost::asio::ip::tcp::iostream s("boost.org","http");
  s.expires_from_now(boost::posix_time::seconds(2));

  // send request
  s<<"GET / HTTP/1.0\r0.2cm/n";
  s<<"Host: www.boost.org\r\n";
  s<<"Accept: /*\r\n";
  s<<"Connection: close\r\n\r\n";

  // get response
  std::string header;
  while(std::getline(s,header)&&header!="\r"){
    std::cout<<header<<std::endl;
  }
  std::cout<<s.rdbuf();
}


// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>

// cpp-netlib generates lots of pedantic warnings with gcc - ignore them
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <boost/network.hpp>
#include <boost/network/include/http/server.hpp>
#include <boost/network/protocol/http/message.hpp>
#pragma GCC diagnostic pop

using namespace std;
namespace http=boost::network::http;

// forward decl
struct handler;
using http_server=http::server<handler>;

// http request handler
struct handler {
  void operator()(http_server::request const&request,http_server::response&response){
    cout<<"uri: "<<request.method<<" from source: "<<request.source<<":"<<request.source_port<<
          ", destination: "<<request.destination<<endl;
    response=http_server::response::stock_reply(http_server::response::ok,"http server running ...");
  }
  void log(http_server::string_type const &info) {
    cerr<<"ERROR: "<<info<<'\n';
  }
};
// main test program
int main(int arg, char * argv[]) {
  handler handler;
  http_server::options options(handler);
  http_server server(options.address("0.0.0.0").port("8000"));
  server.run();
}

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include <boost/asio_queue.hpp>
#include <boost/log/trivial.hpp>
#include <string>
#include <thread>
#include <iostream>
#include <functional>
#include <memory>
#include <unistd.h>

using namespace std;
using namespace std::placeholders;
namespace asio=boost::asio;

// server constants
constexpr int port=7787;
string const server{"localhost"};
constexpr std::size_t maxclients{5};

// ----- queue types -----

// aios io service
asio::io_service ios;

// queue value type
using qval_t=string;

// serializer
std::function<void(std::ostream&,qval_t const&)>serialiser=[](std::ostream&os,qval_t const&s){
  os<<s;
};
// de-serializer
std::function<qval_t(istream&)>deserialiser=[](istream&is){
  string line;
  getline(is,line);
  return line;
};

// queue type
using queue_t=asio::sockclient_queue<qval_t,decltype(deserialiser),decltype(serialiser)>;

// ------ test program
int main(){
  try{
    // create a client queue
    queue_t qclient(server,port,deserialiser,serialiser);
    BOOST_LOG_TRIVIAL(debug)<<"client queue created ...";

    // send a message on queue (will connect to server)
    boost::system::error_code ec;
    qclient.enq("Message from client 0",ec);
    if(ec!=boost::system::error_code()){
      BOOST_LOG_TRIVIAL(error)<<"failed sending message, ec: "<<ec;
      exit(1);
    }
// NOTE!
sleep(2);
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

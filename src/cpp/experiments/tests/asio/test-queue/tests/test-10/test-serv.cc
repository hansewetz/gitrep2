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
constexpr int listenport=7787;
string const server{"localhost"};
constexpr std::size_t maxclients{5};
constexpr std::size_t tmoPollMs{1000};

// ----- queue types -----

// aios io service
asio::io_service ios;

// queue value type
using qval_t=string;

// de-serialize an object (notice: message boundaries are on '\n' characters)
std::function<qval_t(istream&)>deserialiser=[](istream&is){
  string line;
  getline(is,line);
  return line;
};

// queue type
using queue_t=asio::sockdeq_serv_queue<qval_t,decltype(deserialiser)>;

// ------ test program
int main(){
  try{
    // create queues
    queue_t qserv0(listenport,deserialiser,maxclients,tmoPollMs);
    BOOST_LOG_TRIVIAL(debug)<<"server queue created ...";

    // get items from queue
    while(true){
      boost::system::error_code ec;
      BOOST_LOG_TRIVIAL(info)<<"waiting for message ...";
      qserv0.wait_deq(ec);
      BOOST_LOG_TRIVIAL(info)<<"got message ...";
      auto p=qserv0.deq(ec);
      BOOST_LOG_TRIVIAL(info)<<"dequed message: "<<p.second;
    }
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

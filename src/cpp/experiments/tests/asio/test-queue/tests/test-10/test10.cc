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

// ----- queue types -----

// aios io service
asio::io_service ios;

// queue type
using qval_t=string;

// de-serialize an object (notice: message boundaries are on '\n' characters)
std::function<qval_t(istream&)>deserialiser=[](istream&is){
  string line;
  getline(is,line);
  return line;
};

// ------ test program
int main(){
  try{
    // create queues
    asio::sockdeq_serv_queue<qval_t,decltype(deserialiser)>qserv0(listenport,deserialiser,maxclients);
    BOOST_LOG_TRIVIAL(debug)<<"server queue created ...";

sleep(20);

    // setup asio object
    //asio::queue_listener<decltype(qserv0)>qlistener(::ios,&qserv0);
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

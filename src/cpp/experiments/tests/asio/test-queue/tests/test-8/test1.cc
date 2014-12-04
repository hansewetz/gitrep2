// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

/*
This program tests that 'socketserv_queue' works
*/

#include <boost/asio_queue.hpp>
#include <boost/log/trivial.hpp>
#include <string>
#include <functional>
#include <iostream>
using namespace std;
namespace asio=boost::asio;

// queue stuff
using qval_t=string;

// serialize an object (notice: message boundaries are on '\n' characters)
std::function<void(std::ostream&,qval_t const&)>serialiser=[](std::ostream&os,qval_t const&s){
  os<<s;
};
// de-serialize an object (notice: message boundaries are on '\n' characters)
std::function<qval_t(istream&)>deserialiser=[](istream&is){
  string line;
  getline(is,line);
  return line;
};

// global variables
constexpr int listenPort=7787;
constexpr size_t maxclients{2};

// test program
int main(){
  try{
    // create queue
    // (after creation, the queue will accept client connections and act as a full duplex queue with each client)
    asio::sockserv_queue<qval_t,decltype(deserialiser),decltype(serialiser)>qserv(listenPort,deserialiser,serialiser,maxclients);

    // NOTE! Not yet done
    // ...

  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

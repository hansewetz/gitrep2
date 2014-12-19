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

// test program
int main(){
  try{
    // create queue
    // (after creation, the queue will accept client connections and act as a full duplex queue with each client)
    asio::sockserv_queue<qval_t,decltype(deserialiser),decltype(serialiser)>qserv(listenPort,deserialiser,serialiser);

    // send a message
    boost::system::error_code ec1;
    bool ret1{qserv.timed_wait_enq(10000,ec1)};
    if(ec1!=boost::system::error_code()){
      BOOST_LOG_TRIVIAL(error)<<"timed_wait_enq() failed: "<<ec1.message();
      exit(1);
    }
    BOOST_LOG_TRIVIAL(info)<<"can send message";

/*
    // listen for a message
    boost::system::error_code ec2;
    pair<bool,qval_t>ret2{qserv.timed_deq(100000,ec2)};
    if(ec2!=boost::system::error_code()){
      BOOST_LOG_TRIVIAL(error)<<"deque() failed: "<<ec2.message();
      exit(1);
    }
    BOOST_LOG_TRIVIAL(info)<<"got message: \""<<ret2.second<<"\"";
*/

    // NOTE! Not yet done
    // ...
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

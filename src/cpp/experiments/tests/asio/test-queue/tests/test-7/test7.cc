/*
this program tests the 'fd_queue' in isolation
*/

#include <boost/asio_queue.hpp>
#include <boost/log/trivial.hpp>
#include <string>
#include <iostream>
#include <unistd.h>
using namespace std;
namespace asio=boost::asio;

// value type in queues
// (must work with operator<< and operator>>, and be default constructable)
using qval_t=std::string;

// setup serializer/de-serializer for queue + queue-type
function<qval_t(istream&)>deserialiser=[](istream&is){qval_t ret;is>>ret;return ret;};
function<void(ostream&,qval_t const&)>serialiser=[](ostream&os,qval_t const&i){os<<i;};
using queue_t=asio::fd_queue<qval_t,decltype(deserialiser),decltype(serialiser)>;

// test program
int main(){
  try{
    // create a queue
    int fd[2];
    pipe(fd);
    int fdread{fd[0]};
    int fdwrite{fd[1]};
    size_t maxsize{10};
    queue_t q{maxsize,fdread,1,deserialiser,serialiser};
    
    // write a string to fdwrite (input fd to queue
    string s{"Hello there\n"};
    write(fdwrite,s.c_str(),s.length());

    // deq() from queue
    boost::system::error_code ec;
    pair<bool,string>p{q.deq(ec)};
    if(ec!=boost::system::error_code()){
      cout<<"deq() error, ec: "<<ec.message()<<endl;
    }else{
      cout<<"deq(), item: "<<p.second<<endl;
    }
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

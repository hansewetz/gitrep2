/*
NOTE! How should we manage serialisation by using streams instead of file descriptors?
      It seems not possible to flush things to the other side unless we have an 'endl'
      Maybe we should read byte by byte and assemble messages ...

*/
/*
this program tests the 'fd_queue' in isolation
*/

#include <boost/asio_queue.hpp>
#include <boost/log/trivial.hpp>
#include <string>
#include <iostream>
#include <functional>
#include <memory>
#include <unistd.h>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

using namespace std;
namespace asio=boost::asio;
namespace io=boost::iostreams;

// queue type
using qval_t=string;

// serialize an object (notice: message boundaries are on '\n' characters)
std::function<void(ostream&,qval_t const&)>serialiser=[](ostream&os,qval_t const&s){
  os<<s;
};
// de-serialize an object (notice: message boundaries are on '\n' characters)
std::function<qval_t(istream&)>deserialiser=[](istream&is){
  while(true){
    string line;
    getline(is,line);
    if(line=="")break;
    return line;
  }
};
// queue types
using enq_t=asio::fdenq_queue<qval_t,decltype(serialiser)>;
using deq_t=asio::fddeq_queue<qval_t,decltype(deserialiser)>;

// test program
int main(){
  try{
    // create an fd-pipe with connected read/write fds
    int fd[2];
    if(pipe(fd)!=0){
      BOOST_LOG_TRIVIAL(error)<<"pipe failed";
      exit(1);
    }
    int fdread{fd[0]};
    int fdwrite{fd[1]};

    // create queues
    boost::system::error_code ec;
    enq_t qin{fdwrite,serialiser};
    deq_t qout{fdread,deserialiser};

    // send and receive a number of messages
    constexpr size_t maxmsg{100};
    for(size_t i=0;i<maxmsg;++i){
      string msg{boost::lexical_cast<string>(i)};

      // enq() an item
      cout<<">>> \""<<msg<<"\""<<endl;
      qin.enq(msg,ec);

      // deq() an item
      pair<bool,qval_t>tout{qout.deq(ec)};
      if(!tout.first)cout<<"failed deq(), err: "<<ec.message()<<endl;
      else cout<<"<<< \""<<tout.second<<"\""<<endl;
    }
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

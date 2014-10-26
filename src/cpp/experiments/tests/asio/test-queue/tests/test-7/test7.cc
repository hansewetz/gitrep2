/*
this program tests the 'fd_queue' in isolation
*/

#include <boost/asio_queue.hpp>
#include <boost/log/trivial.hpp>
#include <string>
using namespace std;
namespace asio=boost::asio;

// value type in queues
// (must work with operator<< and operator>>, and be default constructable)
using qval_t=std::string;

// setup data so we can create a queue
function<qval_t(istream&)>deserialiser=[](istream&is){qval_t ret;is>>ret;return ret;};
function<void(ostream&,qval_t const&)>serialiser=[](ostream&os,qval_t const&i){os<<i;};
using queue_t=asio::fd_queue<qval_t,decltype(deserialiser),decltype(serialiser)>;

// create a queue
int fdread{0};
int fdwrite{1};
queue_t q{fdread,fdwrite,deserialiser,serialiser};

// test program
int main(){
  try{
    // NOTE! Not yet done
  }
  catch(exception const&e){
    BOOST_LOG_TRIVIAL(error)<<"cought exception: "<<e.what();
  }
}

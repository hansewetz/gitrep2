#include "polldir_queue.h"
#include <string>
#include <functional>

using namespace std;
namespace asio= boost::asio;
namespace fs=boost::filesystem;
namespace pt= boost::posix_time;

// deserialise stream into a string
int reader(istream&is){
  // NOTE!
  return 5;
}
// test main program
int main(){
  function<int(istream&)>read{reader};
  asio::polldir_queue<int,decltype(read)>pq{10,1000,fs::path{".",},read,true};
}

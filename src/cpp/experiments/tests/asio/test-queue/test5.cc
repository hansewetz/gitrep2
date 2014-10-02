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
// serialise stream into a string
int writer(ostream&os,int i){
  os<<i;
}
// test main program
int main(){
  fs::path qdir{"./q1"};

  // setup a queue
  function<int(istream&)>read{reader};
  function<void(ostream&,int)>write{writer};
  asio::polldir_queue<int,decltype(read),decltype(write)>pq{10,1000,qdir,read,write,true};

  // remove locks if they exist
  pq.removeLockVariables(qdir);

  // test enq/deq 
//  cout<<"enq: "<<boolalpha<<pq.enq(17)<<endl;
while(true){
  pair<bool,int>p{pq.deq()};
  cout<<"deq: "<<boolalpha<<"["<<p.first<<","<<p.second<<"]"<<endl;
}
}

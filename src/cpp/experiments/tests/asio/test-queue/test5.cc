#include "polldir_queue.h"
#include <string>
#include <chrono>
#include <thread>
#include <functional>

using namespace std;
namespace asio= boost::asio;
namespace fs=boost::filesystem;
namespace pt= boost::posix_time;

// print usage info and exit
void usage(){
  cerr<<"usage: test5 [-e|-d]"<<endl;
  exit(1);
}
// test main program
int main(int argc,char*argv[]){
  if(argc!=2)usage();
  bool producer;
  if(!strcmp(argv[1],"-e"))producer=true; else
  if(!strcmp(argv[1],"-d"))producer=false;
  else usage();

  // directory for queue
  fs::path qdir{"./q1"};

  // setup a queue
  function<int(istream&)>reader=[](istream&is){int ret;is>>ret;return ret;};
  function<void(ostream&,int)>writer=[](ostream&os,int i){os<<i;};
  asio::polldir_queue<int,decltype(reader),decltype(writer)>pq{10,5000,qdir,reader,writer,true};

  // remove locks if they exist
//  pq.removeLockVariables(qdir);

  // check if we are consumer or producer
  if(producer){
    for(int i=0;i<100;++i){
      pq.enq(i);
      std::chrono::milliseconds tmo(1000);
      std::this_thread::sleep_for(tmo);
    }
  }else{
    while(true){
      pair<bool,int>p{pq.deq()};
      cout<<"deq: "<<boolalpha<<"["<<p.first<<","<<p.second<<"]"<<endl;
    }
  }
}

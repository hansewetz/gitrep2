/*
  test program for polldir_queue.
  program kicks off a sender thread and a receiver thread and terminates when a specified #of messages has been received
*/
#include "polldir_queue.h"
#include <string>
#include <thread>
#include <functional>

using namespace std;
namespace asio= boost::asio;
namespace fs=boost::filesystem;

// sender
template<typename Q>
void sender(Q&q,size_t maxmsg){
  for(int i=0;i<maxmsg;++i){
    q.enq(i);
    std::chrono::milliseconds tmo(1000);
    std::this_thread::sleep_for(tmo);
  }
}
// receiver
template<typename Q>
void receiver(Q&q,size_t maxmsg){
  while(maxmsg!=0){
    pair<bool,int>p{q.deq()};
    cout<<"deq: "<<boolalpha<<"["<<p.first<<","<<p.second<<"]"<<endl;
    --maxmsg;
  }
}
// test main program
int main(int argc,char*argv[]){
  try{
    // directory for queue
    fs::path qdir{"./q1"};

    // setup queue
    function<int(istream&)>reader=[](istream&is){int ret;is>>ret;return ret;};
    function<void(ostream&,int)>writer=[](ostream&os,int i){os<<i;};
    asio::polldir_queue<int,decltype(reader),decltype(writer)>q1{10,5000,qdir,reader,writer,true};

    // remove locks if they exist
    q1.removeLockVariables(qdir);

    // kick off threads for sender/receiver
    size_t maxmsg{10};
    std::thread tsender{[&](){sender(q1,maxmsg);}};
    std::thread treceiver{[&](){receiver(q1,maxmsg);}};

    // join threads
    treceiver.join();
    tsender.join();
  }
  catch(exception const&e){
    cerr<<"caught exception: "<<e.what()<<endl;
    exit(1);
  }
}

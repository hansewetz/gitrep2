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
void sender(Q&pq,size_t maxmsg){
  for(int i=0;i<maxmsg;++i){
    pq.enq(i);
    std::chrono::milliseconds tmo(1000);
    std::this_thread::sleep_for(tmo);
  }
}
// receiver
template<typename Q>
void receiver(Q&pq,size_t maxmsg){
  while(maxmsg!=0){
    pair<bool,int>p{pq.deq()};
    cout<<"deq: "<<boolalpha<<"["<<p.first<<","<<p.second<<"]"<<endl;
    --maxmsg;
  }
}
// test main program
int main(int argc,char*argv[]){
  try{
    // directory for queue
    fs::path qdir{"./q1"};

    // setup a queue
    function<int(istream&)>reader=[](istream&is){int ret;is>>ret;return ret;};
    function<void(ostream&,int)>writer=[](ostream&os,int i){os<<i;};
    asio::polldir_queue<int,decltype(reader),decltype(writer)>pq{10,5000,qdir,reader,writer,true};

    // remove locks if they exist
    pq.removeLockVariables(qdir);

    // kick off threads for sender/receiver
    size_t maxmsg{10};
    std::thread tsend{[&](){sender(pq,maxmsg);}};
    std::thread trecv{[&](){receiver(pq,maxmsg);}};

    // join threads
    trecv.join();
    tsend.join();
  }
  catch(exception const&e){
    cerr<<"caught exception: "<<e.what()<<endl;
    exit(1);
  }
}

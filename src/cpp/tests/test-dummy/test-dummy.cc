#include "bpf_queue/Message.h"
#include "bpf_queue/Utils.h"
#include "bpf_queue/InMemoryQueue.h"
#include "bpf_queue/InMemoryQueueRepository.h"
#include "bpf_queue/PolldirQueue.h"
#include "bpf_queue/FdQueue.h"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <unistd.h>
using namespace std;
using namespace bpf;

// ping function
void ping(shared_ptr<Queue>qsend,shared_ptr<Queue>qrecv){
  static size_t msgcount{0};
  while(true){
    shared_ptr<Message>msg{make_shared<Message>(Message::msgtype_t::REQUEST)};
    cout<<"PING sending: "<<*msg<<endl<<flush;
    qsend->enq(msg);
    msg=qrecv->deq(true);
    cout<<"PING received: "<<*msg<<endl<<flush;
    if((++++msgcount%100)==0){
      cout<<"#message count: "<<msgcount<<endl;
    }
    this_thread::sleep_for(chrono::seconds(1));
  }
}
// pong function
void pong(shared_ptr<Queue>qsend,shared_ptr<Message>msg,string const&id){
  cout<<id<<" received: "<<*msg<<endl<<flush;
  this_thread::sleep_for(chrono::seconds(1));
  msg=make_shared<Message>(Message::msgtype_t::REPLY);
  cout<<id<<" replying: "<<*msg<<endl<<flush;
  qsend->enq(msg);
}

// -----

// make a queue repository
enum class qreptype_t{INMEMORY,POLLDIR,FD,INMEMORY_POLLDIR};
shared_ptr<QueueRepository>getInmemoryQrep(qreptype_t reptype){
  // we'll create 2 queues for this repository
  shared_ptr<Queue>q1{shared_ptr<Queue>(static_cast<Queue*>(0))};
  shared_ptr<Queue>q2{shared_ptr<Queue>(static_cast<Queue*>(0))};

  // queues
  if(reptype==qreptype_t::INMEMORY){
    q1=make_shared<InMemoryQueue>(QueueId("q1"),true);
    q2=make_shared<InMemoryQueue>(QueueId("q2"),true);
  }else
  if(reptype==qreptype_t::POLLDIR){
    constexpr size_t dirqPollRateMs{1000};
    q1=make_shared<PolldirQueue>(QueueId("q1"),true,dirqPollRateMs,"./junk1",true);
    q2=make_shared<PolldirQueue>(QueueId("q2"),true,dirqPollRateMs,"./junk2",false);
  }else
  if(reptype==qreptype_t::FD){
    int fd1[2];
    int fd2[2];
    if(pipe(fd1)<0)THROW_RUNTIME("failed creaing pipe fd1, errno: "<<errno);
    if(pipe(fd2)<0)THROW_RUNTIME("failed creaing pipe fd2, errno: "<<errno);
    q1=make_shared<FdQueue>(QueueId("q1"),true,fd1[0],fd1[1],true);
    q2=make_shared<FdQueue>(QueueId("q2"),true,fd2[0],fd2[1],true);
  }else
  if(reptype==qreptype_t::INMEMORY_POLLDIR){
    constexpr size_t dirqPollRateMs{1000};
    q1=make_shared<PolldirQueue>(QueueId("q1"),true,dirqPollRateMs,"./junk1",true);
    q2=make_shared<InMemoryQueue>(QueueId("q2"),true);
  }else{
    THROW_RUNTIME("invalid qreptype_t in getInmemoryQrep() function");
  }
  // setup queue repository
  shared_ptr<QueueRepository>qrep(make_shared<InMemoryQueueRepository>(QueueRepositoryId("qrep")));
  qrep->addQueue(q1);
  qrep->addQueue(q2);

  // return in-memory queue repository
  return qrep;
}
// test program to check that build environment works
int main(){
  // get queue repository and get q1
  auto qrep=getInmemoryQrep(qreptype_t::FD);
  auto q1=qrep->queue(string("q1"));
  auto q2=qrep->queue(string("q2"));

  // kick off ping in a thread (send on q1, listen on q2)
  thread t1([&]{ping(q1,q2);});

  // kick off listeners on q1 replying with a message on q2 (on which sender is listening)
  vector<thread>services;
  for(size_t i=0;i<20;++i){
    thread thr([=](){listen(q1,[&](shared_ptr<Message>msg){pong(q2,msg,string("pong")+boost::lexical_cast<string>(i));});});
    services.push_back(std::move(thr));
  }
  // wait for threads
  t1.join();
  for(size_t i=0;i<services.size();++i){
    services[i].join();
  }
}

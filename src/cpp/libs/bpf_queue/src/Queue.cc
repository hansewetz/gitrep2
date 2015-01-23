// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "bpf_queue/Queue.h"
#include "bpf_queue/Message.h"
using namespace std;
namespace bpf{

//ctor
Queue::Queue(QueueId const&qid):id_(qid){
}
// get queue id
QueueId const&Queue::id()const{
  return id_;
}
// print operator
ostream&operator<<(ostream&os,Queue const&q){
  return q.print(os);
}
}

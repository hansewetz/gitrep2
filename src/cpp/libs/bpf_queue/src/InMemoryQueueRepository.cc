// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "bpf_queue/InMemoryQueueRepository.h"
#include <iostream>
using namespace std;
namespace bpf{

// print function
ostream&InMemoryQueueRepository::print(ostream&os)const{
  os<<"id: ["<<id()<<"],";
  for(auto const &p:queues_)os<<"["<<*p.second<<"]";
  return os;
}
// check if queue is in repository
bool InMemoryQueueRepository::hasQueue(QueueId const&qid)const{
  return queues_.find(qid)!=queues_.end();
}
// get queue from repository
shared_ptr<Queue>InMemoryQueueRepository::queue(QueueId const&qid)const{
  decltype(queues_)::const_iterator it{queues_.find(qid)};
  if(it==queues_.end())return shared_ptr<Queue>(nullptr);
  return it->second;
}
// add a queue
void InMemoryQueueRepository::addQueue(shared_ptr<Queue>q){
  decltype(queues_)::const_iterator it{queues_.find(q->id())};
  if(it!=queues_.end())THROW_RUNTIME("attempt to add already existing queue: "<<q->id());
  queues_.insert(make_pair(q->id(),q));
}
// remove a queue
void InMemoryQueueRepository::rmQueue(QueueId const&qid){
  decltype(queues_)::iterator it{queues_.find(qid)};
  if(it==queues_.end())THROW_RUNTIME("attempt to remove non-existing queue with id: "<<qid);
  queues_.erase(it);
}
}

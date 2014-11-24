// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "bpf_queue/QueueRepository.h"
#include <iostream>
using namespace std;
namespace bpf{

// ctor
QueueRepository::QueueRepository(QueueRepositoryId const&id):id_(id){
}
// getters
QueueRepositoryId const&QueueRepository::id()const{
  return id_;
}
// print function
ostream&operator<<(ostream&os,QueueRepository const&qr){
  return qr.print(os);
}
}

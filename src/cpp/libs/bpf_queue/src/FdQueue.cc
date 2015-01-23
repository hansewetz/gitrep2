// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "bpf_queue/FdQueue.h"
#include "bpf_queue/Message.h"
#include "bpf_queue/Utils.h"
#include "utils/utility.h"
#include "utils/fileUtils.h"
#include <boost/log/trivial.hpp>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>

using namespace std;
namespace ipc=boost::interprocess;
namespace bpf{

// lock remover
FdQueue::LockRemover::LockRemover(bool removeLocks,std::string const&qname){
  if(removeLocks){
    FdQueue::removeLockVariables(qname);
  }
}
// ctor (must set started)
FdQueue::FdQueue(QueueId const&id,bool start,int fdread,int fdwrite,bool removelocks):
    Queue(id),started_(start),fdread_(fdread),fdwrite_(fdwrite),
    lockRemover(removelocks,id.asString()),
    mtxRead_(ipc::open_or_create,getMutexName(id.asString(),true).c_str()),
    mtxWrite_(ipc::open_or_create,getMutexName(id.asString(),false).c_str()){

  // set non-blocking mode on fds
  if(fdwrite>=0)setfdnonblock(fdwrite);
  if(fdread>=0)setfdnonblock(fdread);
}
// identified of this queue type
string FdQueue::type()const{
  return "FdQueue";
}
// start queue
void FdQueue::start(){
  started_=true;
}
// stop queue
void FdQueue::stop(){
  started_=false;
}
// check if queue is running
bool FdQueue::started()const{
  return started_;
}
// check if queue is empty
bool FdQueue::empty()const{
  // we can't know how many messages there are in queue
  return true;
}
// get #of elements in queue
size_t FdQueue::size()const{
  // we can't know how many messages there are in queue
  return 0;
}
// enqueue a message
bool FdQueue::enq(shared_ptr<Message>msg){
  // lock write side while writing
  ipc::scoped_lock<ipc::named_mutex>lock(mtxWrite_);

  if(!started_)return false;

  // serialise message to file descriptor
  string smsg{serializeXmlMessage(msg)};
  char const*sbuf{smsg.data()};
  size_t writepos{0};
  size_t nleft{smsg.size()};

  // check if read fd is non-negative
  if(fdwrite_<0)THROW_RUNTIME("attempt to write to negative file descriptor in FdQueue::enq");

  // we might not be able to write entire message in one shot so we need to loop over a select
  while(nleft>0){
    // setup for writing
    fd_set output;
    FD_ZERO(&output);
    FD_SET(fdwrite_,&output);
    int maxfd{fdwrite_};

    // wait until we can write
    int n=select(++maxfd,0,&output,0,0);

    // check if errno in select
    if(n<0)THROW_RUNTIME("select() failed, errno: "<<errno);
      
    // at this point we must be able to write
    if(FD_ISSET(fdwrite_,&output)){
      int ret;
      while((ret=write(fdwrite_,&sbuf[writepos],nleft))==-1&&errno==EINTR);
      if(ret<=0)THROW_RUNTIME("failed writing message to FdQueue, errno: "<<errno);
      nleft-=ret;
      writepos+=ret;
    }
  }
  return true;
}
// dequeue a message (block or return if no message)
shared_ptr<Message>FdQueue::deq(bool block){
  // lock write side while writing
  ipc::scoped_lock<ipc::named_mutex>lock(mtxRead_);

  if(!started_)return shared_ptr<Message>(nullptr);

  // check if read fd is non-negative
  if(fdread_<0)THROW_RUNTIME("attempt to read from negative file descriptor in FdQueue::deq");

  // enter select loop
  shared_ptr<Message>ret;
  while(true){
    // setup for reading input
    fd_set input;
    FD_ZERO(&input);
    FD_SET(fdread_,&input);
    int maxfd{fdread_};

    // setup timeout if non-blocking
    struct timeval tmo;
    if(!block){
      tmo.tv_sec=0;
      tmo.tv_usec=0;
    }
    // wait for input or timeout (if block, then do not timeout)
    int n=select(++maxfd,&input,0,0,block?0:&tmo);

    // check if erro in select
    if(n<0)THROW_RUNTIME("select() failed, errno: "<<errno);

    // check if timeout
    if(n==0)return shared_ptr<Message>(nullptr);

    // we must have input on fdread here
    if(FD_ISSET(fdread_,&input)){
      shared_ptr<istream>is{utils::fdistream(fdread_,false)};
      ret=deserializeXmlMessage(*is);
      break;
    }
  }
  return ret;
}
// set file secritor to non blocking mode
void FdQueue::setfdnonblock(int fd){
  int flags{fcntl(fd,F_GETFL)};
  if(flags<0)THROW_RUNTIME("fcntl: could not get fd flags, errno: "<<errno);
  int ret{fcntl(fd,F_SETFL,flags|O_NONBLOCK)};
  if(ret<0)THROW_RUNTIME("fcntl: could not ser fd to bon-clocking state, errno: "<<errno);
}
// get name of mutex variable
string FdQueue::getMutexName(string const&qname,bool reader){
  return qname+"_FdQueue_"+(reader?"READER":"WRITER")+"_MUTEX";
}
// special functions for FdQueue class
void FdQueue::removeLockVariables(string const&qname){
  boost::interprocess::named_mutex::remove(getMutexName(qname,true).c_str());
  boost::interprocess::named_mutex::remove(getMutexName(qname,false).c_str());
}
// print function
ostream&FdQueue::print(ostream&os)const{
  return os<<"type: "<<type()<<"qid: ["<<id()<<"], started: "<<boolalpha<<"fdwrite: "<<fdwrite_<<", fdread: "<<fdread_<<started_;
}
}

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "bpf_queue/PolldirQueue.h"
#include "bpf_queue/Message.h"
#include "bpf_queue/Utils.h"
#include "utils/utility.h"
#include "utils/fileUtils.h"
#include <boost/log/trivial.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <cstdio>
#include <mutex>
using namespace std;
namespace fs=boost::filesystem;
namespace ipc=boost::interprocess;
namespace pt= boost::posix_time;
namespace bpf{

// remove locks when contructed if asked to do so
PolldirQueue::LockRemover::LockRemover(bool removeLocks,fs::path const&dir){
  if(removeLocks){
    PolldirQueue::removeLockVariables(dir);
 }
}
// ctor (must set started)
PolldirQueue::PolldirQueue(QueueId const&id,bool start,size_t pollrateMsec,fs::path const&dir,bool removeLocks):
    Queue(id),started_(start),pollrateMsec_(pollrateMsec),dir_(dir),
    lockRemover(removeLocks,dir),
    mtx_(ipc::open_or_create,getMutexName(dir).c_str()),cond_(ipc::open_or_create,getCondName(dir).c_str()){

  // check that directory exist and that is a directory
  if(!utils::isDirectory(dir))THROW_RUNTIME("path: "<<dir.string()<<" is not a directory when constructing PolldirQueue: "<<id);
}
// identified of this queue type
string PolldirQueue::type()const{
  return "PolldirQueue";
}
// start queue
void PolldirQueue::start(){
  ipc::scoped_lock<ipc::named_mutex>lock(mtx_);
  started_=true;
  cond_.notify_all();
}
// stop queue
void PolldirQueue::stop(){
  ipc::scoped_lock<ipc::named_mutex>lock(mtx_);
  started_=false;
  cond_.notify_all();
}
// check if queue is running
bool PolldirQueue::started()const{
  ipc::scoped_lock<ipc::named_mutex>lock(mtx_);
  return started_;
}
// check if queue is empty
bool PolldirQueue::empty()const{
  ipc::scoped_lock<ipc::named_mutex>lock(mtx_);
  return emptyNolock();
}
// get #of elements in queue
size_t PolldirQueue::size()const{
  ipc::scoped_lock<ipc::named_mutex>lock(mtx_);
  return allFiles().size();
}
// enqueue a message
bool PolldirQueue::enq(shared_ptr<Message>msg){
  {
    ipc::scoped_lock<ipc::named_mutex>lock(mtx_);
    if(!started_)return false;
    writeMessage(msg);
  }
  cond_.notify_all();
  return true;
}
// dequeue a message (block or return if no message)
shared_ptr<Message>PolldirQueue::deq(bool block){
  // loop until we have a message or until someone stops the queue
  ipc::scoped_lock<ipc::named_mutex>lock(mtx_);
  while(started_){
    // return null ptr if queue is stopped
    if(!started_){
      return shared_ptr<Message>(nullptr);
    }
    // get oldest file
    pair<bool,fs::path>oldestFile{nextFile()};
    if(oldestFile.first){
      // convert file into message and remove file
      return readMessage(oldestFile.second);
    }
    // if we are not blocking return nullptr
    if(!block){
      return shared_ptr<Message>(nullptr);
    }
    // sleep with poll intervall or until someone wakes us up
    const pt::ptime abstm{pt::microsec_clock::local_time()+pt::milliseconds(pollrateMsec_)};
    cond_.timed_wait(lock,abstm,[&](){return !emptyNolock()||!started_;});
    if(!started_)break;
  }
  // return null ptr if queue is stopped
  return shared_ptr<Message>(nullptr);
}
// serialise message into a file
void PolldirQueue::writeMessage(shared_ptr<Message>msg)const{
  // create a file from message id
  boost::filesystem::path file{dir_/msg->id().asString()};
  ofstream of{file.string(),ofstream::trunc|ofstream::binary};
  if(!of)THROW_RUNTIME("could not open file for writing: "<<file);
  of<<serializeXmlMessage(msg);
}
// deserialise message from file and remove file
shared_ptr<Message>PolldirQueue::readMessage(boost::filesystem::path const&fullpath)const{
  ifstream is{fullpath.string(),ifstream::binary};
  if(!is)THROW_RUNTIME("could not open file for reading: "<<fullpath.string());
  shared_ptr<Message>ret{deserializeXmlMessage(is)};
  is.close();
  remove(fullpath.string().c_str());
  return ret;
}
// check for empty without locking
bool PolldirQueue::emptyNolock()const{
  pair<bool,fs::path>oldestFile{nextFile()};
  if(!oldestFile.first)return true;
  return false;
}
// get oldest file + manage cache_ if needed
// (must be called when having the lock)
pair<bool,fs::path>PolldirQueue::nextFile()const{
  // only fill cache if cache is empty
  cleanCache();
  fillCache(false);
  if(cache_.empty())return make_pair(false,fs::path());
  pair<bool,fs::path>ret{make_pair(true,cache_.front())};
  cache_.pop_front();
  return ret;
}
// get all files + manage cache if needed
// (must be called when having the lock)
list<fs::path>PolldirQueue::allFiles()const{
  // force filling cache from directory
  cleanCache();
  fillCache(true);
  return cache_;
}
// fill cache of files
// (must be called when having the lock)
void PolldirQueue::fillCache(bool refill)const{
  // trash cache and re-read it if 'refill' is set or if cache is empty
  if(refill||cache_.empty()){
    multimap<time_t,fs::path>sortedFiles{utils::getTsOrderedFiles(dir_)};
    cache_.clear();
    for(auto const&f:sortedFiles)cache_.push_back(f.second);
  }
}
// clean cache from files that do not exist
// (must be called when having the lock)
void PolldirQueue::cleanCache()const{
  list<fs::path>tmpCache;
  for(auto const&p:cache_){
    if(fs::exists(p))tmpCache.push_back(p);
  }
  swap(tmpCache,cache_);
}
// special functions for PolldirQueue class
void PolldirQueue::removeLockVariables(fs::path const&dir){
  boost::interprocess::named_mutex::remove(getMutexName(dir).c_str());
  boost::interprocess::named_condition::remove(getCondName(dir).c_str());
}
// create mutex name from queue name
string PolldirQueue::getMutexName(fs::path const&dir){
  string sdir{dir.string()};
  std::replace_if(sdir.begin(),sdir.end(),[](char c){return c=='/'||c=='.';},'_');
  return sdir;
}
// create conditional variable name from queue name
string PolldirQueue::getCondName(fs::path const&dir){
  string sdir{dir.string()};
  std::replace_if(sdir.begin(),sdir.end(),[](char c){return c=='/'||c=='.';},'_');
  return sdir;
}
// print function
ostream&PolldirQueue::print(ostream&os)const{
  return os<<"type: "<<type()<<"qid: ["<<id()<<"], pollrateMs: "<<pollrateMsec_<<", dir: "<<dir_.string();
}
}

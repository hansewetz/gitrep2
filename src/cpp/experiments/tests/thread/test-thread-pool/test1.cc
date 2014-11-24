// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#define _GLIBCXX_USE_NANOSLEEP
#include <iostream>
#include <list>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <chrono>
#include <atomic>
using namespace std;

// task list
mutex m;
condition_variable c;
typedef function<void()>Task;
list<Task>taskList;

// variable for stopping
atomic_bool stopFlag;

// enq/deq from task list
void enq(Task&&t){
  {
    lock_guard<mutex>lg(m);
    taskList.push_back(t);
  }
  c.notify_one();
}

// loop and execute tasks until stop flag is set
void tdeq(){
  while(true){
    Task t;
    {
      unique_lock<mutex>ul(m);
      while(taskList.empty()){
        c.wait_for(ul,std::chrono::milliseconds(100));
        if(stopFlag)return;
      }
      t=std::move(*taskList.begin());
      taskList.pop_front();
    }
    t();
  }
}

// poll queue until it's empty
// NOTE! we might hang here since a thread may see the stopflag wheile there are still tasks in queue
void waitUntilQueueEmpty(){
  while(true){
    unique_lock<mutex>ul;
    if(taskList.empty())return;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

// main test program
int main(){
  stopFlag=false;
  thread t1(tdeq);
  thread t2(tdeq);
  thread t3(tdeq);
  for(int i=0;i<100;++i){
    Task t([]{cerr<<"Hello: "<<std::this_thread::get_id()<<endl;});
    enq(std::move(t));
  }
  waitUntilQueueEmpty();
  stopFlag=true;
  t1.join();
  t2.join();
  t3.join();
}


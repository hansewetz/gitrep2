// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <atomic>
#include <thread>
#include <assert.h>
std::atomic<bool> x,y;
std::atomic<int> z;
void write_x_then_y(){
  x.store(true,std::memory_order_relaxed);
  y.store(true,std::memory_order_release);
}
void read_y_then_x(){
  while(!y.load(std::memory_order_acquire));
  if(x.load(std::memory_order_relaxed))
    ++z;
}
int main(){
  for(auto i=0;i<(2<<15);++i){
  x=false;
  y=false;
  z=0;
  std::thread a(write_x_then_y);
  std::thread b(read_y_then_x);
  a.join();
  b.join();
  assert(z.load()!=0);
  }
}

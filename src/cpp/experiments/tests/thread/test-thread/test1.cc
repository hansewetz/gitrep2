// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <iostream>
#include <thread>
#include <future>
#include <functional>
using namespace std;

int main(){
  promise<string>p;
  future<string>f=p.get_future();
  function<void(promise<string>&)>func([](promise<string>&p){p.set_value("Hello");});
  thread t1(func,std::ref(p));
  cerr<<"future: "<<f.get()<<endl;
  t1.join();
}


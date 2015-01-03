// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <iostream>
#include <exception>
#include <fstream>
#include <list>
#include <set>
#include <thread>
#include <mutex>
#include <iterator>
#include <vector>
#include <future>
#include <functional>
#include <algorithm>
#include <random>
#include <chrono>
#include <map>
#include <utility>
#include <unistd.h>
#include <type_traits>
#include <queue>
#include <chrono>
#include <ratio>

using namespace std;

class NumberTracker{
public:
  // ctor
  NumberTracker(int m):m_(m),q_(){}

  // add a number
  void add(int val){
    q_.push(val);
    if(static_cast<int>(q_.size())>m_)q_.pop();
  }
  // get reference to vector containing smallest numbers so far
  vector<int>get()const{
    std::vector<int>vret;
    std::priority_queue<int,vector<int>,less<int>>qtmp=q_;
    while(qtmp.size()>0){
      vret.push_back(qtmp.top());
      qtmp.pop();
    }
    return vret;
  }
private:
  int m_;
  std::priority_queue<int,vector<int>,less<int>>q_;
};

int main(){
  // construct a random generator
  std::minstd_rand0 engine;
  std::uniform_int_distribution<int>distr(0,9);
  std::function<int()>gen=std::bind(distr,engine);

  // fill a vector with random numbers
  NumberTracker nt(10);
  vector<int>v;
  for(int i=0;i<30;++i){
    v.push_back(gen());
    nt.add(v.back());
  }
  sort(v.begin(),v.end(),less<int>());
  copy(v.begin(),v.end(),ostream_iterator<int>(cerr," "));
  cerr<<endl;
  vector<int>w=nt.get();
  copy(w.begin(),w.end(),ostream_iterator<int>(cerr," "));
  cerr<<endl;
}

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

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

// maintain data in two priority queues
std::priority_queue<int,vector<int>,greater<int>>qmin;
std::priority_queue<int,vector<int>,less<int>>qmax;

// add a number to median numbers
void addNumber(int val){
  // push numnber on smallest queue
  if(qmin.size()<qmax.size())qmin.push(val);
  else qmax.push(val);

  // if numbers on top of queues are not in correct place, then swap them
  if(qmin.size()&&qmax.size()&&qmin.top()<qmax.top()){
    int vmin=qmin.top(),vmax=qmax.top();
    qmin.pop();
    qmax.pop();
    qmin.push(vmax);
    qmax.push(vmin);
  }
}
// get median
int getMedian(){
  if(qmin.size()==qmax.size())return (qmin.top()+qmax.top())/2;
  if(qmin.size()>qmax.size())return qmin.top();
  return qmax.top();
}

int main(){
  // construct a random generator
  std::minstd_rand0 engine;
  std::uniform_int_distribution<int>distr(0,9);
  std::function<int()>gen=std::bind(distr,engine);

  // fill a vector with random numbers
  vector<int>v;
  for(int i=0;i<10;++i){
    v.push_back(gen());
    addNumber(v.back());
  }
  // print out sorted vector + median
  std::sort(v.begin(),v.end());
  copy(v.begin(),v.end(),std::ostream_iterator<int>(cerr," "));
  cerr<<endl<<"median: "<<getMedian()<<endl;
}

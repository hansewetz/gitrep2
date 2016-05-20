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

std::priority_queue<int,vector<int>,greater<int>>qmin;  // contains the high values
std::priority_queue<int,vector<int>,less<int>>qmax;     // contains the low values

void add(int val){
  if(qmin.size()==qmax.size()){
    if(qmin.size()>0&&val>qmin.top()){
      qmin.push(val);
    }
    else{
      qmax.push(val);
    }
  }
  else{
    if(qmin.size()>0&&val>qmin.top()){
      if(qmin.size()>qmax.size()){
        int tmp=qmin.top();
        qmax.push(tmp);
        qmin.pop();
      }
      qmin.push(val);
    }
    else{
      if(qmin.size()<qmax.size()){
        int tmp=qmax.top();
        qmin.push(tmp);
        qmax.pop();
      }
      qmax.push(val);
    }
  }
  // this can probably be done smarter
  if(qmin.size()>0&&qmax.size()>0&&qmax.top()>qmin.top()){
    int tmp1=qmin.top(),tmp2=qmax.top();
    qmin.pop();
    qmax.pop();
    qmin.push(tmp2);
    qmax.push(tmp1);
  }
}
int median(){
  if(qmin.size()==0&&qmax.size()==0)throw runtime_error("attempt to get median when no values are available");
  if(qmin.size()==qmax.size())return (qmin.top()+qmax.top())/2;
  if(qmin.size()>qmax.size())return qmin.top();
  return qmax.top();
}

int main(){
  try{
    std::minstd_rand0 engine;
    std::uniform_int_distribution<int>distr(0,9);
    std::function<int()>gen=std::bind(distr,engine);
    vector<int>v;
    for(int i=0;i<41;++i){
      v.push_back(gen());
      add(v.back());
    }
    cerr<<endl;
    int med=median();
    cerr<<"median: "<<med<<endl;
    sort(v.begin(),v.end());
    copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
    cerr<<endl<<"qmin size: "<<qmin.size()<<", qmax.size(): "<<qmax.size()<<endl;

    cerr<<"qmin"<<endl;
    while(qmin.size()){
      cerr<<qmin.top()<<" ";
      qmin.pop();
    }
    cerr<<endl<<"qmax"<<endl;
    while(qmax.size()){
      cerr<<qmax.top()<<" ";
      qmax.pop();
    }
  }
  catch(std::exception&e){
    cerr<<"caught exception: "<<e.what()<<endl;
  }
}

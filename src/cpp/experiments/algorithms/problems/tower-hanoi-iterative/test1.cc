// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

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
#include <cassert>
#include <cstring>
#include <array>
#include <stack>

using namespace std;

void move(stack<int>&from,stack<int>&to){
  if(from.empty()){
    cout<<"from empty"<<endl;
  }
  if(!to.empty()&&from.top()>to.top()){
    cout<<"from > to"<<endl;
    exit(1);
  }
  to.push(from.top());
  from.pop();
}
void hanoi(stack<int>&from,stack<int>&to){
  stack<int>temp;
  stack<int>*stacks[3]{&from,&to,&temp};
  int smallInd{0};
  size_t n{from.size()};
  while(to.size()!=n){
    //cout<<"from: "<<from.size()<<", to: "<<to.size()<<", temp: "<<temp.size()<<endl;

    // move smallest top-disk one step clockwise
    int newSmallInd{(smallInd+1)%3};
    move(*stacks[smallInd],*stacks[newSmallInd]);
    smallInd=newSmallInd;

    // move second smallest disk to larger disk
    stack<int>&s1(*stacks[(smallInd+1)%3]);
    stack<int>&s2(*stacks[(smallInd+2)%3]);
    if(s1.empty()&&s2.empty()){
      // we have all disks on temp
      continue;
    }else
    if(s1.empty()){
      move(s2,s1);
    }else
    if(s2.empty()){
      move(s1,s2);
    }else
    if(s1.top()<s2.top()){
      move(s1,s2);
    }else{
      move(s2,s1);
    }
  }
}
void printTower(stack<int>&t){
  while(t.size()){
    cout<<t.top()<<" ";
    t.pop();
  }
}
int main(){
  stack<int>from;
  stack<int>to;
  for(int i=10;i>=0;--i)from.push(i);
  hanoi(from,to);
  printTower(to);
  cout<<endl;
}

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
#include <cassert>
#include <cstring>
#include <array>
#include <stack>

using namespace std;

void sortStack(stack<int>&s1,stack<int>&s2){
  if(s1.empty())return;
  s2.push(s1.top());
  s1.pop();
  while(!s1.empty()){
    if(s2.top()>=s1.top()){
      s2.push(s1.top());
      s1.pop();
    }
    else{
      int tmp=s1.top();
      s1.pop();
      while(!s2.empty()&&s2.top()<tmp){
        s1.push(s2.top());
        s2.pop();
      }
      s2.push(tmp);
    }
  }
}

// test program
int main(){
  vector<int>v{5,2,8,6,4,5,9,12,3};
  stack<int>s1;
  for_each(v.begin(),v.end(),[&s1](int val){s1.push(val);});
  stack<int>s2;
  sortStack(s1,s2);
  while(!s2.empty()){
    cerr<<s2.top()<<" ";
    s2.pop();
  }
  cerr<<endl;
}

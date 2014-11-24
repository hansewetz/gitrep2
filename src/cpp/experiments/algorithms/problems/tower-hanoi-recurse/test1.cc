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

void towerMove(stack<int>&from,stack<int>&to,stack<int>&temp,int n){
  if(n==1){
    to.push(from.top());
    from.pop();
    return;
  }
  towerMove(from,temp,to,n-1);
  towerMove(from,to,temp,1);
  towerMove(temp,to,from,n-1);
}
void tower(stack<int>&from,stack<int>&to){
  stack<int>temp;
  towerMove(from,to,temp,from.size());
}
void printStack(stack<int>&s,string const&str,ostream&os){
  os<<str;
  while(!s.empty()){
    os<<s.top()<<" ";
    s.pop();
  }
  os<<endl;
}

int main(){
  stack<int>from;
  stack<int>to;
  for(int i=9;i>=0;--i)from.push(i);
  tower(from,to);
  printStack(to,"to: ",cout);
}

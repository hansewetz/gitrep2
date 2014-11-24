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

using namespace std;

int addints(int&a,int&b){
  if(b==0)return a;
  int sum=a^b;
  int carry=(a&b)<<1;
  return addints(sum,carry);
}

int subtract(int a,int b){
  b=~b+1;
  return addints(a,b);
}

int main(){
  int a=1044;
  int b=1000;
  cout<<"a: "<<a<<", b: "<<b<<endl;
  cerr<<"subtract: "<<subtract(a,b)<<endl;
}

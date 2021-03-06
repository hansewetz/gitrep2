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

int main(){
  int a=5;
  int b=8;
  cout<<"a: "<<a<<", b: "<<b<<endl;
  cerr<<"addints: "<<addints(a,b)<<endl;
}

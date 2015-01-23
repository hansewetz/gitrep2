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

using namespace std;

void swapints(int&a,int&b){
  a=a^b;
  b=a^b;
  a=a^b;
}

int main(){
  int a=5;
  int b=8;
  cout<<"a: "<<a<<", b: "<<b<<endl;
  swap(a,b);
  cout<<"a: "<<a<<", b: "<<b<<endl;
}

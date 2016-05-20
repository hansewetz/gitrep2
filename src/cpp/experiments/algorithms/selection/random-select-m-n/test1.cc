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
#include <stack>
#include <stdlib.h>

using namespace std;
using vec=vector<int>;

// select m elements randomly out of n elements
vec selectRandom(int m,int n){
  vec ret;
  for(int i=0;i<n;++i){
    if((rand()%(n-i))<m){
      ret.push_back(i);
      --m;
    }
  }
  return ret;
}

// test program
int main(){
  vec vrand=selectRandom(10,8);
  copy(vrand.begin(),vrand.end(),ostream_iterator<int>(cout," "));
  cout<<endl;
}

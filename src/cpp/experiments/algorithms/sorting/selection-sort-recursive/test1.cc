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

void ssort(vector<int>&v,int l,int u){
  if(l>=u)return;
  int minind=l;
  for(int i=l+1;i<=u;++i){
    if(v[i]<v[minind])minind=i;
  }
  swap(v[l],v[minind]);
  ssort(v,l+1,u);
}

int main(){
  vector<int>v{5,2,7,9,4,5,5,6,4,3,6,4};
  ssort(v,0,v.size()-1);
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
}

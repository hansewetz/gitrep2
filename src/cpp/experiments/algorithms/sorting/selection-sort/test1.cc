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

using namespace std;

void ssort(vector<int>&v){
  int n=v.size();
  for(int i=0;i<n;++i){
    int ismall=i;
    for(int j=i+1;j<n;++j){ // find smallest element
      if(v[j]<v[ismall])ismall=j;
    }
    if(i!=ismall)swap(v[i],v[ismall]);
  }
}
int main(){
  vector<int>v{8,2,6,4,6,89,4,4};
  ssort(v);
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
}

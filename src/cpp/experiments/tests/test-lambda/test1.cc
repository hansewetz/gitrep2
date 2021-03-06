// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
using namespace std;

// main.
int main(int argc,char**argv){
  function<void(int)>f([](int i){cerr<<i<<endl;});
  vector<int>v{3,1,8,5,3,4,9,7};
  sort(v.begin(),v.end());
  for(auto i:v)f(i);
}


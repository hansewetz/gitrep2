// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <random>
#include <functional>
using namespace std;

void isort(vector<int>&v){
  int n=v.size();
  for(int i=1;i<n;++i){
    int t=v[i];
    int j=i;
    for(;j>0&&v[j-1]>t;--j)v[j]=v[j-1];
    v[j]=t;
  }
}
int main(){
  vector<int>v;

  // generate random numbers
  uniform_int_distribution<int>distr(0,1000);
  mt19937 engine;
  auto generator=bind(distr,engine);
  for(int i=0;i<10;++i)v.push_back(generator());

  // sort them
  isort(v);

  // print them
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
  cerr<<endl;
}

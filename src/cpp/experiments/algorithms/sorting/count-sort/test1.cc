// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <random>
#include <boost/timer/timer.hpp>

using namespace std;

// count sort
// (count[i] ==> record i should be in place count[i])
void csort(vector<int>&v){
  int n=v.size();
  vector<int>count(n);

  // count #of items before other items
  for(int i=0;i<n-1;++i){
    for(int j=i+1;j<n;++j){
      if(v[i]>v[j])++count[i];
      else ++count[j];
    }
  }
  // arrange items in sorted order
  vector<int>res(n);
  for(int i=0;i<n;++i){
    res[count[i]]=v[i];
  }
  swap(res,v);
}

// test main program
int main(){
  vector<int>v{4,6,1,2,9,2,66,45,3,89};
  csort(v);
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
}

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iterator>
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

// bubble sort
// (scompare if (K(k+1) < K(k) swap them, repeat)
void bsort(vector<int>&v){
  size_t n{v.size()};
  if(n<=1)return;
  --n;
  for(int i=n-1;i>=0;--i){
    for(int j=0;j<=i;++j){
      if(v[j+1]<v[j])swap(v[j+1],v[j]);
    }
  }
}
int main(){
  vector<int>v{5,2,7,3,4,5,98,24};
  bsort(v);
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
}

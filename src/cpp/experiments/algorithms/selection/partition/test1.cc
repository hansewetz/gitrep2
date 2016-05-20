// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
using namespace std;

int partition(vector<int>&v,int l,int u){
  int m=l;
  int t=v[m];
  for(int i=l+1;i<=u;++i){
    if(v[i]<t)swap(v[++m],v[i]);
  }
  swap(v[l],v[m]);
  return m;
}
int main(){
  vector<int>v={8,6,4,9,57,1,3,2,10,2,26};
  int p=partition(v,0,v.size()-1);
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
  cerr<<endl<<"p: "<<p<<endl;
}

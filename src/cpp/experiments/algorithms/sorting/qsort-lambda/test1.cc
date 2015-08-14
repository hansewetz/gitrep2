// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include <iostream>
#include <functional>
#include <algorithm>
#include <iterator>
using namespace std;

int main(){
  std::function<void(vector<int>&,int,int)>qsort=[&qsort](vector<int>&v,int l,int u){
    if(l>=u)return;
    int m=l;
    int t=v[l];
    for(int i=l+1;i<=u;++i){
      if(v[i]<t)swap(v[i],v[++m]);
    }
    swap(v[m],v[l]);
    qsort(v,l,m-1);
    qsort(v,m+1,u);
  };

  vector<int>v{8,2,5,4,8,5,3};
  qsort(v,0,v.size()-1);
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
  cout<<endl;
}

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

int partition(vector<int>&v,int l,int u){
  int m=l;
  int t=v[l];
  for(int i=l;i<=u;++i){
    if(v[i]<t)swap(v[++m],v[i]);
  }
  swap(v[m],v[l]);
  return m;
}
int kthsmallest(vector<int>&v,int k){
  k-=1;
  if(k<0||k>=v.size())throw runtime_error("kth element outside range of vector");
  int l=0;
  int u=v.size()-1;
  while(l<=u){
    int p=partition(v,l,u);
    if(k==p)return v[p];
    if(k<p)u=p-1;
    else   l=p+1;
  }
}
int median(vector<int>&v){
  int n=v.size();
  if(n==1)return v[0];
  if(n==2)return (v[0]+v[1])/2;
  int k1=kthsmallest(v,(n-1)/2+1);	// only need to do both when even number of elements
  int k2=kthsmallest(v,n/2+1);
  return (k1+k2)/2;
}

int main(){
  vector<int>v{5,2,8,9,1,3};
  sort(v.begin(),v.end());
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
  cerr<<"median: "<<median(v)<<endl;
}

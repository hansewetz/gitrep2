// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// check if there are two entrie sin an array which sums to x
bool checkSum(vector<int>v,int x){
  sort(v.begin(),v.end());
  int l=0;
  int r=v.size()-1;
  while(l<r){
    int sum=v[l]+v[r];
    if(sum==x)return true;
    if(sum<x)++l;
    else --r;
  }
  return false;
}
// test program
int main(){
  vector<int>v{1,6,4,5,9,6,10};
  int x=77;
  cout<<boolalpha<<"result: "<<checkSum(v,x)<<endl;
}

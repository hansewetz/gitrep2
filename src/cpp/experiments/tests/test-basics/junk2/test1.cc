#include <stdlib.h>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <list>
using namespace std;

// find max
int findmax(vector<int>const&v,size_t ind){
  if(ind==v.size()-1)return v[ind];
  int lmax=findmax(v,ind+1);
  if(v[ind]>lmax)return v[ind];
  return lmax;
}

// test program
int main(){
  vector<int>v{4,7,2,10,77,0,2,40};
  cout<<findmax(v,0)<<endl;
}

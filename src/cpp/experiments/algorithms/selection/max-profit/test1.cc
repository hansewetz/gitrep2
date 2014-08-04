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

// find the largest distance between two elements in an array under the condition
// that the smallest element is before the largest element.
// this is the same as maximum possible profit when buying a share and selling it later given
// that the vector contains the prices when we can buy/sell

int maxProfitAux(vector<int>&v,int l,int u,int&vmin,int&vmax){
  int n=u-l+1;
  if(n==1){
    vmin=vmax=v[l];
    return 0;
  }
  // partition and get profit, max and min from each partition
  int m=(u+l)/2;
  int vmin1,vmax1,vmin2,vmax2;
  int p1=maxProfitAux(v,l,m,vmin1,vmax1);
  int p2=maxProfitAux(v,m+1,u,vmin2,vmax2);
  int p3=(vmax2>vmin1)?(vmax2-vmin1):0;

  // set return values and return
  vmin=min(vmin1,vmin2);
  vmax=max(vmax1,vmax2);
  return max(p3,max(p1,p2));
}
int maxProfit(vector<int>&v){
  if(v.size()==0)return 0;
  int vmin,vmax;
  return maxProfitAux(v,0,v.size()-1,vmin,vmax);
}

int main(){
  // construct a random generator
  std::minstd_rand0 engine;
  std::uniform_int_distribution<int>distr(5,30);
  std::function<int()>gen=std::bind(distr,engine);

  // fill a vector with random numbers
  vector<int>v;
  for(int i=0;i<15;++i)v.push_back(gen());

  // print vector
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
  cerr<<endl;
  cerr<<"maxProfit: "<<maxProfit(v)<<endl;

}

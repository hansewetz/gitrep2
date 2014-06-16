#include <iostream>
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

using namespace std;

int main(){
  std::function<void(vector<int>&,int,int)>qsort=[&qsort](vector<int>&v,int l,int u)->void{
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
}

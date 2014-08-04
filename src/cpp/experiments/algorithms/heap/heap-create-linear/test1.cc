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
#include <stack>

using namespace std;

namespace{
  int swapCount=0;
}

// sink item ind within the array v
void sink(vector<int>&v,int ind){
  int n=v.size()-1;
  while(ind<=n/2){
    // get child indices
    int c1ind=ind*2;
    int c2ind=ind*2+1;

    // select child
    int cind;
    if(c2ind<=n)cind=(v[c1ind]<v[c2ind])?c1ind:c2ind;
    else cind=c1ind;
 
    // make sure we are within bounds
    if(cind>n)break;
    
    // sink element if needed
    if(v[cind]<v[ind]){
      swap(v[cind],v[ind]);
      ++swapCount;
    }
    else break;
    ind=cind;
  }
}
// linear alg for creating a heap (no need to float element)
void createHeap(vector<int>&v){
  int n=v.size()-1;
  for(int i=n/2;i>0;--i){
    sink(v,i);
  }
}
// test program
int main(){
  vector<int>v{-1,5,2,8,7,4,7,10,18};
  createHeap(v);
  vector<int>::iterator it=v.begin();
  advance(it,1);
  cout<<"is_heap: "<<boolalpha<<is_heap(it,v.end(),greater<int>())<<endl;
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
  cout<<endl<<"swapCount: "<<swapCount<<endl;
}

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

using namespace std;

// Note: if streams are sorted with small elements first, then priority queue must have small elements on top.

// merge m sorted streams
vector<int>mergeStreams(vector<int>*va[],int m){
  vector<int>vret;
  std::priority_queue<int,vector<int>,greater<int>>qmin;

  // get indices for vectors
  vector<int>ind(m,0);

  // insert first element from from all streams in qmin
  for(int i=0;i<m;++i){
    if(ind[i]<va[i]->size()){
      qmin.push((*va[i])[ind[i]]);
      ind[i]=ind[i]+1;
    }
  }
  // loop until qmin is empty
  while(qmin.size()){
    vret.push_back(qmin.top());
    qmin.pop();
    for(int i=0;i<m;++i){
      if(ind[i]<va[i]->size()){
        qmin.push((*va[i])[ind[i]]);
        ind[i]=ind[i]+1;
      }
    }
  }
  return vret;
}

int main(){
  // construct a random generator
  std::minstd_rand0 engine;
  std::uniform_int_distribution<int>distr(0,9);
  std::function<int()>gen=std::bind(distr,engine);

  // fill a vector with random numbers
  vector<int>v1,v2,v3;
  vector<int>*va[]{&v1,&v2,&v3};
  int ind=0;
  for(int i=0;i<30;++i){
    (*va[ind]).push_back(gen());
    ind=(ind+1)%3;
  }
  for(int i=0;i<3;++i)sort(va[i]->begin(),va[i]->end());

  vector<int>vsorted=mergeStreams(va,3);
  cerr<<"#numbers: "<<vsorted.size()<<endl;
  copy(vsorted.begin(),vsorted.end(),ostream_iterator<int>(cerr," "));
}

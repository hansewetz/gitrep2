// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

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
#include <regex>

using namespace std;

// P struct + print operator
struct P{
  P(int h,int w):height(h),weight(w){}
  P()=default;
  int height;
  int weight;
};
ostream&operator<<(ostream&os,P const&p){
  return os<<"("<<p.height<<", "<<p.weight<<")";
}
// comparison function for Ps
function<bool(P const&,P const&)>cmpless=[](P const&p1, P const&p2)->bool{if(p1.height<p2.height)return true;if(p1.height>p2.height)return false;return p1.weight<p2.weight;};

// get longest path starting from an index increasing (height,weight) (returns next start index)
int getLongestPathWithStartInd(vector<P>const&v,int start,vector<P>&vmax){
  // initialization
  int retind=-1;      // if ret==-1, then no more slots to try from
  int n=v.size();

  // push back first element
  vmax.push_back(v[start]);
  for(int i=start+1;i<n;++i){
    // check if element is larger than previous element
    if(v[i].height>=v[i-1].height&&v[i].weight>=v[i-1].weight){
      vmax.push_back(v[i]);
    }else{
      if(retind!=-1)retind=i;    // keep only lowest return index
    }
  }
  return retind;
}
// find longest path of increasing (height,weight)
vector<P>getLongestPath(vector<P>const&v){
  vector<P>vin(v);
  sort(vin.begin(),vin.end(),cmpless);
  vector<P>maxpath;
  int maxsize=maxpath.size();
  int nextind=0;
  while(nextind>=0){
    vector<P>tmppath;
    nextind=getLongestPathWithStartInd(vin,nextind,tmppath);
    if(maxpath.size()<tmppath.size())swap(maxpath,tmppath);
  }
  return maxpath;
}
// test program
int main(){
  vector<P>v;
  v.push_back(P(160,70));
  v.push_back(P(198,90));
  v.push_back(P(198,40));
  v.push_back(P(170,130));
  v.push_back(P(150,50));
  v.push_back(P(150,70));
  v.push_back(P(183,72));
  v.push_back(P(182,90));
  v.push_back(P(182,60));
  vector<P>maxpath=getLongestPath(v);
  cout<<"maxlen: "<<maxpath.size()<<" --> ";
  copy(maxpath.begin(),maxpath.end(),ostream_iterator<P>(cout," "));
  cout<<endl;
}

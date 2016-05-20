// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

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


// print all subsets of a vector
void generateSubsetsAux(vector<int>&v,vector<bool>&vb,int k,int n){
  // check if we have a solution
  if(k==n){
    cout<<"{";
    for(int i=0;i<n;++i){
        if(vb[i])cout<<v[i]<<" ";
    }
    cout<<"}"<<endl;
  }
  else{
    // fill in next element in solution, then fill in remaining elements by recursive call
    vb.push_back(true);                  // add new element to soliution
    generateSubsetsAux(v,vb,k+1,n);      // generate all solutions from this point
    vb.pop_back();                       // undo move

    // fill in next element in solution, then fill in remaining elements by recursive call
    vb.push_back(false);                 // add new element
    generateSubsetsAux(v,vb,k+1,n);      // generate all solutions from this point
    vb.pop_back();                       // undo move
  }
}
void generateSubsets(vector<int>&v,int k,int n){
  vector<bool>vb;
  generateSubsetsAux(v,vb,0,v.size());
}
int main(){
  vector<int>v{1,2,3,4};
  generateSubsets(v,0,v.size());
}

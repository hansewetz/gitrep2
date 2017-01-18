#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <functional>
#include <algorithm>
#include <iterator>
#include <memory>
#include <mutex>
#include <thread>
#include <future>
#include <type_traits>
#include <sstream>
using namespace std;

// generate all paths from position [0, 0] --> position [N, M] in a grid
constexpr int nrows=5,ncols=6;
void path(int row,int col,vector<pair<int,int>>&v){
  if(row==nrows-1&&col==ncols-1){
    for(size_t i=0;i<v.size();++i){
      cout<<"["<<v[i].first<<", "<<v[i].second<<"]";
    }
    cout<<endl;
    return;
  }
  // try walking down
  if(row!=nrows-1){
    v.push_back(make_pair(row+1,col));
    path(row+1,col,v);
    v.pop_back();
  }
  if(col!=ncols-1){
    v.push_back(make_pair(row,col+1));
    path(row,col+1,v);
    v.pop_back();
  }
}
void genpaths(){
  vector<pair<int,int>>v;
  path(0,0,v);
}

// test program
int main(){
  genpaths();
}

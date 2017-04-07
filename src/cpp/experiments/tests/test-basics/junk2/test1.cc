/*
  - DONE: shortest path (BFS)
  - DONE: topological sort using DFS 
  - DONE: shortest path with weights (DIJKSTRA)
  - Floyd-Warshal alg. - shortest distance between any nodes (i,j)
  - shortest path with function (DP)
  - DONE: longest common subsequence (LCS - DP)
  - longest palindrome which is a subsequence of a string (LCS + LCS-transpose)
  - Ford-Fulkerson maximum flow algorithm
*/
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <queue>
#include <map>
#include <set>
#include <typeinfo>
#include <climits>
using namespace std;

// calculate LCS
int lcs(vector<int>const&v1,vector<int>const&v2){
  // initialize
  size_t n1=v1.size();
  size_t n2=v2.size();
  vector<vector<int>>table(n1);
  for(size_t i=0;i<n1;++i)table<i>=vector<int>(n2,0);

  // evaluate from bottom up
  for(size_t i=1;i<n1;++i){
    
  }
}

// test program
int main(){
  vector<int>v1{1,2,3,4,5,6,7,8,9};
  vector<int>v2{1,3,5,7,18,88,9};
  cout<<"lcs: "<<lcs(v1,v2)<<endl;
}

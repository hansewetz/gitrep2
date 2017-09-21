/*
  - DONE: minsort vector
  - DONE: mergesort vector
  - DONE: isort vector
  - DONE: msort list
  - DONE: shortest path (BFS)
  - DONE: longest common subsequence (LCS - DP)
  - DONE: qsort
  - DONE: permutation
  - DONE: subsets
  - DONE: kth smallest value

HERE-->
  - DONE: DFS: print layered
  - DONE: BFS: top sort
  - DONE: Dijkstra: shortest dist in weighted graph
  - DONE: BFS shortest distance
  - DONE: detect cycle in graph
  - Bellman-Ford shortest distance with negavive weights
  - DFS find stringly-connected components (groups of vertices that can be reached) (relativly easy)
  - Transitive closure of graph (http://www.geeksforgeeks.org/transitive-closure-of-a-graph/)


  - Floyd-Warshal alg. - shortest distance between any nodes (i,j)
  - shortest path with function (DP)
  - longest palindrome which is a subsequence of a string (LCS + LCS-transpose)
  - Ford-Fulkerson maximum flow algorithm
  - consequitive sequence with maximum sum
*/
#include <bits/stdc++.h>

using namespace std;

// generate all paths from (0,0) --> (M,N) in a grid by walking right or down
// (basic backtracking)
void genpaths(vector<pair<int,int>>&path,int row,int col,vector<vector<bool>>&used,int nrows,int ncols){
  // check if we are done
  if(row==nrows-1&&col==ncols-1){
    for(auto const&p:path){
     cout<<"("<<p.first<<","<<p.second<<")";
    }
    cout<<endl;
    return;
  }
  // try stepping right
  if(col!=ncols-1){
    if(!used[row][col+1]){
      used[row][col+1]=true;
      path.push_back(make_pair(row,col+1));
      genpaths(path,row,col+1,used,nrows,ncols);
      path.pop_back();
      used[row][col+1]=false;
    }
  }
  // try stepping down
  if(row!=nrows-1){
    if(!used[row+1][col]){
      used[row+1][col]=true;
      path.push_back(make_pair(row+1,col));
      genpaths(path,row+1,col,used,nrows,ncols);
      path.pop_back();
      used[row+1][col]=false;
    }
  }
}
// test program
int main(){
  int nrows=3;
  int ncols=4;
  vector<pair<int,int>>path;
  vector<vector<bool>>used(nrows,vector<bool>(ncols,false));
  genpaths(path,0,0,used,nrows,ncols);
}

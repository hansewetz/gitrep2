/*
  - DONE: dijkstra graph alg.
  - DONE: minsort vector
  - DONE: mergesort vector
  - DONE: isort vector
  - msort list
  - DONE: shortest path (BFS)
  - DONE: topological sort using DFS 
  - DONE: shortest path with weights (DIJKSTRA)
  - DONE: longest common subsequence (LCS - DP)
  - DONE: qsort
  - DONE: permutation
  - DONE: subsets
  - Floyd-Warshal alg. - shortest distance between any nodes (i,j)
  - shortest path with function (DP)
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

using graph=vector<vector<pair<int,int>>>;


// test program
int main(){
  graph g{
    {{1,1},{2,1}},
    {{3,1}},
    {{4,1}},
    {{3,1}},
    {}
  };
  auto dist=dijk(g,0);
  copy(begin(dist),end(dist),ostream_iterator<int>(cout," "));
  cout<<endl;
}

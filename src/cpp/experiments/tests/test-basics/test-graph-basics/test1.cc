#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <functional>
#include <algorithm>
#include <iterator>
using namespace std;

// graph in bfs order
void bfs(int start,vector<vector<int>>const&g,function<void(int)>fdisc,function<void(int,int)>fedge,vector<int>&parents){
  parents=vector<int>(g.size(),-1);
  vector<bool>discovered(g.size(),false);
  vector<bool>processed(g.size(),false);
  queue<int>q;
  q.push(start);
  discovered[start]=true;          // we have found the vertex
  while(!q.empty()){
    int u=q.front();
    q.pop();
    fdisc(u);
    vector<int>const&adj=g[u];
    for(auto v:adj){
      if(!processed[v])fedge(u,v); // ensures a bidirectional edge is processed exactly ones
      if(discovered[v])continue;   // if have already found vertex it already is (or has been) in the queue or 
      parents[v]=u;
      q.push(v);
      discovered[v]=true;
    }
    processed[u]=true;             // processed==true means that all outbound edges have been processed
  }
}
// print path from node 0 and another node
void printdfspath(int n,vector<int>const&parents){
  // if parents == -1 we are at the start node
  // otherwise we print the path from start node to parnet of this node
  if(parents[n]!=-1){
    printdfspath(parents[n],parents);
  }
  cout<<n<<endl;
}
// graph in dfs order
void dfs_nonrec(int start,vector<vector<int>>const&g,function<void(int)>fdisc){
  vector<int>discovered(g.size(),false);
  stack<int>st;
  st.push(start);
  discovered[start]=true;
  while(!st.empty()){
    int u=st.top();
    st.pop();
    fdisc(u);
    vector<int>const&adj=g[u];
    for(auto v:adj){
      if(discovered[v])continue;
      st.push(v);
      discovered[v]=true;
    }
  }
}
// recursive - returns max depth
int dfs_rec(int start,vector<vector<int>>const&g,function<void(int)>fdisc,vector<bool>&discovered){
  if(discovered[start])return 0;
  discovered[start]=true;
  fdisc(start);
  vector<int>const&adj=g[start];
  int maxdepth=0;
  for(auto v:adj){
    if(discovered[v])continue;
    maxdepth=max(dfs_rec(v,g,fdisc,discovered),maxdepth);
  }
  return maxdepth+1;
}
// do a topological sort of a graph
void topsort(vector<vector<int>>const&g){
  // caclulate in-degree for each vertex
  int nverts=g.size();
  vector<int>indegree(nverts,0);
  for(int i=0;i<nverts;++i){
    vector<int>const&adj=g[i];
    for(int j:adj)++indegree[j];
  }
  // push all vertices with in-dgree==0 into a queue
  queue<int>q;
  for(int i=0;i<nverts;++i){
    if(indegree[i]==0)q.push(i);
  }
  // loop until queue is not empty
  int count=0;
  while(!q.empty()){
    int u=q.front();
    q.pop();
    cout<<u<<endl;
    ++count;
    vector<int>const&adj=g[u];
    for(int v:adj){
      if(--indegree[v]==0)q.push(v);
    }
  }
  // check that we got all vertices in graph
  if(count!=nverts)cout<<"ERROR: graph cannot be sorted topological"<<endl;
}
// test program
int main(){
  // edges: g[0]-->g[0][0]...g[0][n] all edges from node 0
  vector<vector<int>>g{
    {1,3},
    {2,4},
    {5},
    {2,5},
    {5},
    {}};

  // ------------ bfs tests --------------------
  cout<<"------------ bfs tests --------------------"<<endl;
  // process a vertex
  function<void(int)>fdisc=[](int u){};

  // process an edge
  function<void(int,int)>fedge=[](int u,int v){cout<<"["<<u<<", "<<v<<"]"<<endl;};

  // do bfs traversal
  vector<int>parents;
  bfs(0,g,fdisc,fedge,parents);

  // print parent array
  copy(begin(parents),end(parents),ostream_iterator<int>(cout," "));
  cout<<endl;

  // print path from vertex 0 --> another vertex
  printdfspath(5,parents);

  // ------------ dfs tests --------------------
  cout<<"------------ dfs tests --------------------"<<endl;
  cout<<"dfs non-recursive"<<endl;
  dfs_nonrec(0,g,[](int u){cout<<u<<endl;});
  cout<<"dfs recursive"<<endl;
  vector<bool>discovered(g.size(),false);
  int maxdepth=dfs_rec(0,g,[](int u){cout<<u<<endl;},discovered);
  cout<<"maxdepth: "<<maxdepth<<endl;

  // ------------ topsort --------------------
  cout<<"------------ topsort test --------------------"<<endl;
  topsort(g);
}

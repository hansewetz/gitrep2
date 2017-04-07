#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <functional>
#include <algorithm>
#include <iterator>
#include <set>
#include <climits>
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
// do a bfs and collect the last vertices - will get in topsort in reverse order
void topsort1(vector<vector<int>>const&g,vector<bool>&discovered,int nstart,vector<int>&ret){
  if(discovered[nstart])return;
  discovered[nstart]=true;
  for(int v:g[nstart]){
    if(discovered[v])continue;
    topsort1(g,discovered,v,ret);
  }
  ret.push_back(nstart);
}
// do a topsort from each vertex - we don't know where to start
vector<int>topsort1(vector<vector<int>>const&g){
  size_t n=g.size();
  vector<int>ret;
  vector<bool>discovered(n,false);
  for(size_t i=0;i<n;++i){
    topsort1(g,discovered,i,ret);
  }
  reverse(begin(ret),end(ret));
  return ret;
}
// tropsort with color-dfs
void topsort_color(vector<vector<int>>const&g,int nstart,vector<int>&color,vector<int>&res){
  color[nstart]=1;
  for(auto const&v:g[nstart]){
    if(color[v]==0){
      topsort_color(g,v,color,res);
    }else
    if(color[v]==1){
      cerr<<"error - cycle detected from: "<<nstart<<" to "<<v<<endl;
    }else{
      // cross edge - nothing to do
    }
  }
  res.push_back(nstart);
  color[nstart]=2;
}
vector<int>topsort_color(vector<vector<int>>const&g){
  vector<int>res;
  vector<int>color(g.size(),0);
  for(size_t i=0;i<g.size();++i){
    if(color[i]==0)topsort_color(g,i,color,res);
  }
  reverse(begin(res),end(res));
  return res;
}

// Dijkstras alg (shortest path) using 'set' as priority queue
// (adj list: vector[vert, edge-weight])
vector<int>dijpath(vector<vector<pair<int,int>>>const&g,int nstart){
  auto n(g.size());
  vector<int>dist(n,INT_MAX);
  dist[nstart]=0;

  // create a set storing vertices that takes dist into account
  auto cmp=[&dist](int u,int v){return make_pair(dist[u],u)<make_pair(dist[v],v);};
  set<int,decltype(cmp)>q(cmp);

  // start it ...
  q.insert(nstart);
  while(!q.empty()){
    int u=*q.begin();
    q.erase(q.begin());

    // recalculate distance for each adj vertex
    for(auto const&p:g[u]){
      int v=p.first;
      int edist=p.second;
      int newdist=dist[u]+edist;
      if(newdist<dist[v]){
        if(q.count(v))q.erase(v); // remove v from priority q
        dist[v]=newdist;          // update distance for v
        q.insert(v);              // insert v again (dist[v] has been updated)
      }
    }
  }
  return dist;
}
// Dijstras alg without using a priority queue
vector<int>dpath_noq(vector<vector<pair<int,int>>>const&g,int nstart){
  size_t n=g.size();
  vector<int>dist(n,INT_MAX);
  vector<bool>used(n,false);
  dist[nstart]=0;
  while(true){
    // find vertex with lowest distance to nstart
    int mindist=INT_MAX;
    int umin=-1;
    for(size_t i=0;i<n;++i){
      if(!used[i]&&dist[i]<mindist){
        mindist=dist[i];
        umin=i;
      }
    }
    if(umin==-1)break;
    used[umin]=true;

    // relax distances from each adjacent vertex
    for(auto const&p:g[umin]){
      int v=p.first;
      int weight=p.second;
      int newdist=dist[umin]+weight;
      if(newdist<dist[v])dist[v]=newdist;
    }
  }
  return dist;
}
// detect cycle in grapgh using dfs + white/grey/black coloring
constexpr const int white=0;   // not seen
constexpr const int grey=1;    // being processed
constexpr const int black=2;   // processed
bool dfs_findcycle(vector<vector<pair<int,int>>>const&g,int nstart,vector<int>&color){
  color[nstart]=grey;
  for(auto const&p:g[nstart]){
    int v=p.first;
    if(color[v]==white){
      if(dfs_findcycle(g,v,color))return true;
    }else
    if(color[v]==grey){
      return true;
    }else{
      // black - we have already processed vertex
      continue;
    }
  }
  color[nstart]=black;
  return false;
}
bool dfs_findcycle(vector<vector<pair<int,int>>>const&g,int nstart){
  vector<int>color(g.size(),white);
  return dfs_findcycle(g,nstart,color);
}
// code Bellman-Ford alg
// (handles negative edges)
decltype(auto)bellford(vector<vector<pair<int,int>>>const&g,int nstart){
  size_t n=g.size();
  vector<int>dist(n,INT_MAX);
  dist[nstart]=0;

  // iterate n-1 times
  for(size_t i= 0;i<n-1;++i){
    // relax all edges (skip edges that have a start vertex with INT_MAX dist)
    for(size_t u=0;u<n;++u){
      // no point relaxing u if we don't have a distance to it
      if(dist[u]==INT_MAX)continue;

      for(auto const&p:g[u]){
        // relax dist to this vertex
        int v=p.first;
        int w=p.second;
        int newdist=dist[u]+w;
        if(newdist<dist[v])dist[v]=newdist;
      }
    }
  }
  return dist;
}
// find 'bridge-edge' in graph --> edge that cuts a bidirectional graph into two graphs
// (use 'visit number' in dfs to determine if there is a bridge in a non-directed graph)
// (if the return value (lowest visit-number) is higher than current visit-number, we have can cut the graph here and get two graphs)
int dfs_bidirectional(vector<vector<int>>const&g,int u,vector<int>&color,vector<int>&vnum,int&num,int parent){
  color[u]=1;
  vnum[u]=num++;
  int lownum=num;
  auto const&adj=g[u];
  for(auto const&v:adj){
    if(v==parent)continue;
    if(color[v]==0){ // non-discovered vertex (grey-->white)
      int newnum=dfs_bidirectional(g,v,color,vnum,num,u);
      if(newnum>vnum[u]){
        cout<<"bridge from : "<<u<<" --> "<<v<<endl;
      }
      lownum=min(lownum,newnum);
    }else{ // back-edge (grey-->grey)
      lownum=min(lownum,vnum[v]);
    }
  }
  return lownum;
}
void dfs_bidirectional(vector<vector<int>>const&g,int u){
  vector<int>color(g.size(),0);
  vector<int>vnum(g.size(),0);
  int num=0;
  dfs_bidirectional(g,u,color,vnum,num,-1);
}
// -------------- find shortest path in a DAG
// dfs used for topsort
void dfs_with_wights(vector<vector<pair<int,int>>>const&g,int u,vector<int>&color,vector<int>&res){
  color[u]=1;
  for(auto const&p:g[u]){
    int v=p.first;
    if(color[v]!=0)continue;
    dfs_with_wights(g,v,color,res);
  }
  color[u]=2;
  res.push_back(u);
}
// do a top sort
vector<int>topsort_with_weights(vector<vector<pair<int,int>>>const&g){
  vector<int>color(g.size(),0);
  vector<int>res;
  for(size_t u=0;u<g.size();++u){
    if(color[u]!=0)continue;
    dfs_with_wights(g,u,color,res);
  }
  reverse(begin(res),end(res));
  return res;
}
// relax an edge
bool relax_edge(int u,int v,int w,vector<int>&dist){
  int newdist=dist[u]+w;
  if(newdist<dist[v]){
    dist[v]=newdist;
    return true;
  }
  return false;
}
// find shortest path from start node to end node using topsort
// (returns shortest length of path)
int dag_short_path(vector<vector<pair<int,int>>>const&g,int ustart,int uend){
  auto tv=topsort_with_weights(g);
  vector<int>dist(g.size(),INT_MAX);
  dist[ustart]=0;
  for(int u:tv){
    for(auto const&p:g[u]){
      int v=p.first;
      int w=p.second;
      relax_edge(u,v,w,dist);
    }
  }
  return dist[uend];
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
  cout<<"------------ topsort1 test --------------------"<<endl;
  topsort1(g);

  cout<<"------------ Dijktsras shortest (using set as priorty queue) --------------------"<<endl;
  // adj lists are [vertex, edge-wight]
  vector<vector<pair<int,int>>>ge{
    {{1,1},{2,1}},
    {{3,1}},
    {{4,2}},
    {{5,2},{5,1}},
    {{5,2}},
    {}
  };
  auto dist=dijpath(ge,0);
  copy(begin(dist),end(dist),ostream_iterator<int>(cout," "));
  cout<<endl;

  cout<<"------------ Dijktsras shortest (no use of priority queue) --------------------"<<endl;
  auto dist1=dpath_noq(ge,0);
  copy(begin(dist1),end(dist1),ostream_iterator<int>(cout," "));
  cout<<endl;

  cout<<"------------ detecte cycle (use dfs + white/grey/black coloring) --------------------"<<endl;
  auto hascycle=dfs_findcycle(ge,0);
  cout<<boolalpha<<hascycle<<endl;

  cout<<"------------ Bell-Ford --------------------"<<endl;
  auto dist2=bellford(ge,0);
  copy(begin(dist2),end(dist2),ostream_iterator<int>(cout," "));
  cout<<endl;
  cout<<"------------ topsort (dfs-color) --------------------"<<endl;
  auto topsort_color_res=topsort_color(g);
  copy(begin(topsort_color_res),end(topsort_color_res),ostream_iterator<int>(cout," "));
  cout<<endl;

  // test 'bridge-find' in bi-directional graph
  cout<<"------------ use dfs to find bridge edges in bi-ditrectional graph --------------------"<<endl;
  vector<vector<int>>gb{
    {1,2},
    {0,2},
    {0,1,3},
    {2,4},
    {3}
  };
  dfs_bidirectional(gb,0);

  // find shortest path in a DAG (do top sort followed by relax of edges in topsort order)
  cout<<"------------ find shortest path in a DAG (do top sort followed by relax of edges in topsort order"<<endl;
  vector<vector<pair<int,int>>>gdag{
    {{1,2},{2,1}},
    {{3,6}},
    {{4,5}},
    {{5,4}},
    {{5,4}},
    {}
  };
  int dag_shortest_path=dag_short_path(gdag,0,5);
  cout<<"dag_short_path: "<<dag_shortest_path<<endl;
}

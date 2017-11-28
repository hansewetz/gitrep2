#include <bits/stdc++.h>
using namespace std;

// graph type
using graph_t=vector<vector<int>>;
enum color_t{white=0,grey=1,black=2};


bool hascycleAux(graph_t const&g,size_t ustart,vector<color_t>&color){
  color[ustart]=grey;
  for(int v:g[ustart]){
    if(color[v]==grey)return true;
    if(color[v]==black)continue;
    return hascycleAux(g,v,color);
  }
  color[ustart]=black;
  return false;
}
bool hascycle(graph_t const&g){
  auto n=g.size();
  vector<color_t>color(n,white);
  for(size_t u=0;u<n;++u){
    if(hascycleAux(g,u,color))return true;
  }
  return false;
}

// test program
int main(){
  graph_t g{{1,2},{3},{3},{}};
  cout<<"has-cycle: "<<boolalpha<<hascycle(g)<<endl;
}

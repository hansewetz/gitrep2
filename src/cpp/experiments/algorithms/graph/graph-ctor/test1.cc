// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "graph.h"
using namespace std;

int main(){
  const int nvertices=8;
  Graph g(nvertices);
  for(int i=0;i<nvertices;++i)g.addEdge(i,(i+1)%nvertices);
  cout<<g<<endl;
}

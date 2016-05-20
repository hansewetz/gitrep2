// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#ifndef __GRAPH_H__
#define __GRAPH_H__
#include <vector>
#include <iostream>

class Graph{
public:
  friend std::ostream&operator<<(std::ostream&os,Graph&g){return g.print(os);}
  Graph(int nverts){
    for(auto i=0;i<nverts;++i)vert_.push_back(std::vector<int>());
  }
  void addEdge(int v, int w){
    ++nedges_;
    vert_[v].push_back(w);
    vert_[w].push_back(v);
  }
  int nedges()const{return nedges_;}
  int nvertices()const{return vert_.size();}
  std::vector<int>const&adjVertices(int v)const{
    return vert_[v];
  }
private:
  std::ostream&print(std::ostream&os)const{
    os<<"nvertices: "<<nvertices()<<std::endl;
    os<<"nedges: "<<nedges()<<std::endl;
    for(auto it=vert_.begin();it!=vert_.end();++it){
      os<<distance(vert_.begin(),it)<<": ";
      for(auto vit=it->begin();vit!=it->end();++vit){
        os<<*vit<<" ";
      }
      os<<std::endl;
    }
    return os;
  }
  int nedges_;
  std::vector<std::vector<int>>vert_;
};
#endif

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <iostream>
using namespace std;

class Node{
public:
  Node(int v):next(0),val(v){}
  void append(int v){
    Node*n=this;
    while(n->next)n=n->next;
    n->next=new Node(v);
  }
  void print(){
    for(Node*n=this;n;n=n->next){
      cerr<<"val: "<<n->val<<endl;
    }
  }
private:
  Node*next;
  int val;
};

int main(){
  Node n(1);
  n.append(2);
  n.append(3);
  n.print();
}

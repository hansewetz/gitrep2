// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <queue>
using namespace std;

// node
struct node{
  node(int v,node*l,node*r):val(v),left(l),right(r){}
  int val;
  node*left;
  node*right;
};
// print in leveled order
void printTree(node*root){
  queue<node*>q;
  queue<node*>qnext;

  q.push(root);
  while(!q.empty()){
    // get next in q and pop it
    node*n=q.front();
    q.pop();

    // if popped node is not null print it and add it's children to nextq
    if(n!=0){
      cout<<n->val<<" ";
      qnext.push(n->left);
      qnext.push(n->right);
    }
    // check if we are done at this level
    if(q.empty()){
      cout<<endl;
      swap(q,qnext);
    }
  }
}
int main(){
  // build a tree
  node*n10=new node(10,0,0);
  node*n9=new node(9,n10,0);
  node*n7=new node(7,0,0);
  node*n8=new node(8,n9,n7);
  node*n18=new node(18,0,0);
  node*n17=new node(17,n18,0);
  node*n3=new node(3,n8,n17);
  printTree(n3);
}

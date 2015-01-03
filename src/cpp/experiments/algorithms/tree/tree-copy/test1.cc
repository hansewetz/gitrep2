// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <iostream>
#include <exception>
#include <fstream>
#include <list>
#include <set>
#include <thread>
#include <mutex>
#include <iterator>
#include <vector>
#include <future>
#include <functional>
#include <algorithm>
#include <random>
#include <chrono>
#include <map>
#include <utility>
#include <unistd.h>
#include <type_traits>
#include <queue>
#include <chrono>
#include <ratio>
#include <cassert>
#include <cstring>
#include <array>
#include <stack>
#include <regex>

using namespace std;

// tree node
struct node{
  node(int v):val(v),left(nullptr),right(nullptr){}
  int val;
  node*left;
  node*right;
};
// create a tree from a packed array
node*createTree(vector<std::size_t>&v){
  if(v.size()==0)return nullptr;

  // create tree using BFS
  queue<node*>q;
  std::size_t ind=0;
  node*root=new node(v[ind++]);
  q.push(root);
  while(!q.empty()){
    node*p=q.front();
    q.pop();
    // take care of one level on left side
    if(ind<v.size()){
      node*n=new node(v[ind++]);
      q.push(n);
      p->left=n;
    }
    // take care of one level on right side
    if(ind<v.size()){
      node*n=new node(v[ind++]);
      q.push(n);
      p->right=n;
    }
  }
  return root;
}
// print tree level-order (cannot be done recursivly - must use queue)
void printLevelOrder(node*n){
  if(n==nullptr)return;
  queue<node*>q;
  q.push(n);
  while(!q.empty()){
    node*n=q.front();
    q.pop();
    cout<<n->val<<" ";
    if(n->left)q.push(n->left);
    if(n->right)q.push(n->right);
  }
}
// deep copy of tree
node*deepCopyRecurse(node*n){
  if(n==nullptr)return n;
  node*ret=new node(n->val);
  ret->left=deepCopyRecurse(n->left);
  ret->right=deepCopyRecurse(n->right);
  return ret;
}
// deep copy of tree
node*deepCopyIter(node*n){
  if(n==nullptr)return n;

  // do pre-order traversal - simplest
  // whenever we push a node on the stack we also make a copy of it
  stack<node*>st;
  stack<node*>stcopy;
  st.push(n);
  node*ncopy=new node(n->val);
  node*ret=ncopy;
  stcopy.push(ncopy);
  while(!st.empty()){
    n=st.top();
    st.pop();
    ncopy=stcopy.top();
    stcopy.pop();
    if(n->left){
      ncopy->left=new node(n->left->val);
      st.push(n->left);
      stcopy.push(ncopy->left);
    }
    if(n->right){
      ncopy->right=new node(n->right->val);
      st.push(n->right);
      stcopy.push(ncopy->right);
    }
  }
  return ret;
}
// test program
int main(){
  vector<std::size_t>v{1,2,3,4,5,6,7,8,9,10,11};
  node*root=createTree(v);
  printLevelOrder(root);
  cerr<<endl;
  node*copy1=deepCopyRecurse(root);
  printLevelOrder(copy1);
  cerr<<endl;
  node*copy2=deepCopyIter(root);
  printLevelOrder(copy2);
  cerr<<endl;
}

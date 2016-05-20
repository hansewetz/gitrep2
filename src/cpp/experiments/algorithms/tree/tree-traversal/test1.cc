// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

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
  node(size_t v):val(v),left(nullptr),right(nullptr){}
  size_t val;
  node*left;
  node*right;
};
// create a tree from a packed array
node*createTree(vector<size_t>&v){
  if(v.size()==0)return nullptr;

  // create tree using BFS
  queue<node*>q;
  size_t ind=0;
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
// print tree pre-order
void printPreOrder(node*n){
  if(n==nullptr)return;
  cout<<n->val<<" ";
  printPreOrder(n->left);
  printPreOrder(n->right);
}
// print tree pre-order non-recursive
void printPreOrderIterate(node*n){
  if(n==nullptr)return;
  stack<node*>st;
  st.push(n);
  while(!st.empty()){
    n=st.top();
    cout<<n->val<<" ";
    st.pop();
    if(n->right)st.push(n->right); // careful to push right tree first
    if(n->left)st.push(n->left);   // and left tree second
  }
}
// print tree in-order non-recursive
// (must keep a flag for eached pushed node so we don't push it's children more than ones)
void printInOrderIterate(node*n){
  if(n==nullptr)return;
  using stp=pair<node*,bool>;
  stack<stp>st;
  st.push(make_pair(n,false));
  while(!st.empty()){
    stp p=st.top();
    st.pop();
    node*n=p.first;
    bool hasPushedChilder=p.second;
    // push right, middle then left --> we'll pop left next time around
    if(!hasPushedChilder&&n->right)st.push(make_pair(n->right,false));
    if(!hasPushedChilder&&n->left){
      st.push(make_pair(n,true));
      st.push(make_pair(n->left,false));
    }
    else{
      // when we have already pushed left tree or no left tree then print node
      cout<<n->val<<" ";
    }
  }
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
// test program
int main(){
  vector<size_t>v{1,2,3,4,5,6,7,8,9,10,11};
  node*root=createTree(v);
  cout<<"preorder (recurse): ";
  printPreOrder(root);
  cout<<endl;
  cout<<"preorder    (iter): ";
  printPreOrderIterate(root);
  cout<<endl;
  cout<<"level       (iter): ";
  printLevelOrder(root);
  cout<<endl;
  cout<<"inorder     (iter): ";
  printInOrderIterate(root);
  cout<<endl;
}

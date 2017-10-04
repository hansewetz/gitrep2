/*
 - remove a node from a binary tree
 - find max value in a binary tree
*/


#include <bits/stdc++.h>
using namespace std;

struct node{
  node(int v,node*l=0,node*r=0):val(v),left(l),right(r){}
  int val;
  node*left;
  node*right;
};
// create a tree from a packed array
node*vec2tree(vector<int>const&v){
  size_t n=v.size();
  if(n==0)return nullptr;
  size_t ind=0;
  queue<node*>q;
  node*ret=new node(v[ind++]);
  q.push(ret);
  while(!q.empty()){
    node*u=q.front();
    q.pop();
    if(ind<n){
      u->left=new node(v[ind++]);
      q.push(u->left);
    }
    if(ind<n){
      u->right=new node(v[ind++]);
      q.push(u->right);
    }
  }
  return ret;
}
// deep copy - preorder non-reurse
node*deepcp(node*t){
  if(!t)return 0;
  auto ret=new node(t->val);
  stack<node*>st;
  stack<node*>st1;
  st.push(t);
  st1.push(ret);
  while(!st.empty()){
    auto n=st.top();
    auto n1=st1.top();
    st.pop();
    st1.pop();
    if(n->right){
      n1->right=new node(n->right->val);
      st.push(n->right);
      st1.push(n1->right);
    }
    if(n->left){
      n1->left=new node(n->left->val);
      st.push(n->left);
      st1.push(n1->left);
    }
  }
  return ret;
}
// print tree in in-order traversal
void print(node*t){
  if(!t)return;
  print(t->left);
  cout<<t->val<<" ";
  print(t->right);
}
// test program
int main(){
  vector<int>v{0,1,2,3,4,5,6,7};
  auto t=vec2tree(v);
  auto t1=deepcp(t);
  print(t);
  cout<<endl;
  print(t1);
  cout<<endl;
}

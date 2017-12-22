#include <bits/stdc++.h>
using namespace std;

struct tnode{
  tnode*left;
  tnode*right;
  int val;
};

// print binary tree in sorted order
void pbtreesorted(tnode*root){
  if(root==nullptr)return;
  pbtreesorted(root->left);
  cout<<root->val<<endl;
  pbtreesorted(root->right);
}

// create a tree from a packed array (recursivly)
tnode*treefrompackedvecRecursive(vector<int>const&v,int ind){
  if(ind>=static_cast<int>(v.size()))return nullptr;
  tnode*left=treefrompackedvecRecursive(v,2*(ind+1)-1);
  tnode*right=treefrompackedvecRecursive(v,2*(ind+1));
  return new tnode{left,right,v[ind]};
}
tnode*treefrompackedvecRecursive(vector<int>const&v){
  if(v.size()==0)return nullptr;
  return treefrompackedvecRecursive(v,0);
}
// create a tree from a packed array
tnode*treefrompackedvec(vector<int>const&v){
  if(v.size()==0)return nullptr;
  int ind=0;
  int u=v.size()-1;
  tnode*ret=new tnode{nullptr,nullptr,v[ind++]};
  queue<tnode*>q;
  q.push(ret);
  while(!q.empty()){
    tnode*n=q.front();
    q.pop();
    if(ind<=u){
      tnode*left=new tnode{nullptr,nullptr,v[ind++]};
      q.push(left);
      n->left=left;
    }
    if(ind<=u){
      tnode*right=new tnode{nullptr,nullptr,v[ind++]};
      q.push(right);
      n->right=right;
    }
  }
  return ret;
}

// print tree in level order
void plevel(tnode*root){
  if(root==nullptr)return;
  queue<tnode*>q;
  queue<tnode*>qtmp;
  q.push(root);
  while(!q.empty()){
    tnode*n=q.front();
    q.pop();
    cout<<n->val<<" ";
    if(n->left)qtmp.push(n->left);
    if(n->right)qtmp.push(n->right);
    if(q.empty()){
      cout<<endl;
      qtmp.swap(q);
    }
  }
}
// print tree in bfs order
// (should be the same as level order)
void pbfs(tnode*root){
  if(root==nullptr)return;
  queue<tnode*>q;
  queue<tnode*>qtmp;
  q.push(root);
  while(!q.empty()){
    tnode*n=q.front();
    q.pop();
    if(!n)continue;
    cout<<n->val<<" ";
    qtmp.push(n->left);
    qtmp.push(n->right);
    if(q.empty()){
      q.swap(qtmp);
      cout<<endl;
    }
  }
}
// build a tree binary search tree from a sorted array
tnode*treefromsortvec(vector<int>const&v,int l,int u){
  if(l>u)return nullptr;
  int m=(l+u)/2;
  return new tnode{treefromsortvec(v,l,m-1),treefromsortvec(v,m+1,u),v[m]};
}
tnode*treefromsortvec(vector<int>const&v){
  return treefromsortvec(v,0,v.size()-1);
}
// make a deep copy of tree
tnode*deepcp(tnode*t){
  if(t==nullptr)return nullptr;
  return new tnode{deepcp(t->left),deepcp(t->right),t->val};
}
// calculate max height in tree
int maxheight(tnode*t){
  if(!t)return 0;
  return max(maxheight(t->left),maxheight(t->right))+1;
}
// test program
int main(){
  vector<int>v{0,1,2,3,4,5,6,7};

  // create a binary sort tree
  //auto troot=treefromsortvec(v,0,static_cast<int>(v.size())-1);
  //pbtreesorted(troot);

  // crteate a tree where the levels from left to right follows elements in a vector
  //auto troot=treefrompackedvec(v);
  auto troot=treefrompackedvecRecursive(v);
  //plevel(troot);
  //pbfs(troot);

  // test deepcp
  tnode*trootcp=deepcp(troot);
  pbfs(trootcp);

  // print some measures on tree
  cout<<"maxheight: "<<maxheight(troot)<<endl;
}

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <list>
#include <queue>
#include <stack>
using namespace std;

// basic tree structure
struct node{
  struct node*left;
  struct node*right;
  int val;
};
// create a tree from a packed array
node*treefromarray(vector<int>const&v,size_t p){
  if(p>=v.size())return 0;
  node*left=treefromarray(v,2*(p+1)-1);
  node*right=treefromarray(v,2*(p+1));
  return new node{left,right,v[p]};
}
// create a tree from a packed array
node*treefromarraybfs(vector<int>const&v){
  size_t ind=0;
  size_t maxind=v.size();
  list<node*>q;
  node*root=new node{0,0,v[ind++]};
  q.push_front(root);
  while(!q.empty()){
    node*n=q.back();
    q.pop_back();
    if(ind<maxind){
      node*nl=new node{0,0,v[ind++]};
      q.push_front(nl);
      n->left=nl;
    }
    if(ind<maxind){
      node*nr=new node{0,0,v[ind++]};
      q.push_front(nr);
      n->right=nr;
    }
  }
  return root;
}
// print in bfs orde - line by line
void printbfs(node*root){
  queue<node*>q;
  q.push(root);
  queue<node*>q1;
  while(!q.empty()){
    auto n=q.front();
    q.pop();
    if(n==0)continue;
    cout<<n->val<<" ";
    q1.push(n->left);
    q1.push(n->right);
    if(q.empty()){
      q.swap(q1);
      cout<<endl;
    }
  }
}
// print tree in pre-order
void preorder(node*root){
  if(root==0)return;
  cout<<root->val<<endl;
  preorder(root->left);
  preorder(root->right);
}
// print inorder
void printinorder(node*n){
  if(n==0)return;
  printinorder(n->left);
  cout<<n->val<<" ";
  printinorder(n->right);
}
// find height of a tree
size_t maxheight(node*n){
  if(n==0)return 0;
  return 1+max(maxheight(n->left),maxheight(n->right));
}
// find min-heigt in a tree
int minheight(node*n){
  if(n==0)return 0;
  int lh=minheight(n->left);
  int rh=minheight(n->right);
  return 1+min(lh,rh);
}
// find max sum of one path in tree
int maxpathsum(node*n){
  if(n==0)return 0;
  int lsum=maxpathsum(n->left);
  int rsum=maxpathsum(n->right);
  return n->val+max(lsum,rsum);
}
// get minimukm sukm of a full path in tree
int minpathsum(node*n){
  if(n==0)return 0;
  int lsum=minpathsum(n->left);
  int rsum=minpathsum(n->right);
  return n->val+min(lsum,rsum);
}
// make a deep copy of a tree
node*deepcp(node*t){
  if(t==0)return 0;
  node*nl=deepcp(t->left);
  node*nr=deepcp(t->right);
  return new node{nl,nr,t->val};
}
// find longest increasing path in tree
int maxincrpath(node*n,node*nprev,int maxsofar,int max2here){
  // if node null, the max path is 0
  if(n==0)return maxsofar;

  // update max2here and maxso far for current node
  if(n->val>nprev->val)++max2here;
  else max2here=1;
  if(max2here>maxsofar)maxsofar=max2here;

  // check left/right side
  int lmax=0;
  if(n->left)lmax=maxincrpath(n->left,n,maxsofar,max2here);
  int rmax=0;
  if(n->right)rmax=maxincrpath(n->right,n,maxsofar,max2here);

  // calculate return value
  return max(maxsofar,max(lmax,rmax));
}
int maxincrpath(node*n){
  // add a dummy node since we need a previous node
  node ndummy{n,0,-1};
  return maxincrpath(n,&ndummy,0,0);
}
void line(string const&s){
  cout<<"----------------------------- "<<s<<endl;
}
// create binary tree for sorted array
node*mkbintree(vector<int>&v,int l,int u){
  if(l>u)return nullptr;
  int m=(l+u)/2;
  node*ln=mkbintree(v,l,m-1);
  node*rn=mkbintree(v,m+1,u);
  return new node{ln,rn,v[m]};
}
// create a sorted vector from a binary tree
void tree2vector(node*root,vector<int>&v){
  if(root==0)return;
  tree2vector(root->left,v);
  v.push_back(root->val);
  tree2vector(root->right,v);
}
// find max value in  binary search tree
int maxval(node*tree){
  if(tree==0)return -1;
  if(tree->right==0)return tree->val;
  return maxval(tree->right);
}
// traverse tree in-order non-recursivly
void inorder_nonrecursive(node*root){
  if(root==0)return;
  stack<pair<node*,bool>>st;
  st.push(make_pair(root,false));
  while(!st.empty()){
    auto p=st.top();
    node*n=p.first;
    bool haspushed=p.second;
    st.pop();
    // if we have not pushed childern yet and right node != 0
    if(!haspushed&&n->right){
      st.push(make_pair(n->right,false));
    }
    // if we have not pushed childern yet and left node != 0
    if(!haspushed&&n->left){
      st.push(make_pair(n,true));
      st.push(make_pair(n->left,false));
    }else{
      cout<<n->val<<" ";
    }
  }
}
// test program
int main(){
  vector<int>v{0,1,2,3,4,5,6,7,8,9};
  node*root1=treefromarray(v,0);
  line("bfs");
  printbfs(root1);
  node*root2=treefromarraybfs(v);
  line("bfs");
  printbfs(root2);
  line("preorder");
  preorder(root2);
  node*ncp=deepcp(root2);
  line("copy bfs");
  printbfs(ncp);
  line("misc");
  cout<<"maxheight: "<<maxheight(root2)<<endl;
  cout<<"minheight: "<<minheight(root2)<<endl;
  cout<<"maxsum: "<<maxpathsum(root2)<<endl;
  cout<<"minsum: "<<minpathsum(root2)<<endl;
  cout<<"maxincrpath: "<<maxincrpath(root1)<<endl;

  // create binary tree from sorted array
  line("binary tree");
  vector<int>vs{0,1,2,3,4,5,6,7,8};
  node*btree=mkbintree(vs,0,vs.size()-1);
  printinorder(btree);
  cout<<endl;

  // create a sorted vector from  binary search tree
  vector<int>vt{0,1,2,3,4,5,6,7,8};
  node*trees=mkbintree(vt,0,vt.size()-1);
  vector<int>vu;
  tree2vector(trees,vu);
  copy(begin(vu),end(vu),ostream_iterator<int>(cout," "));
  cout<<endl;

  // traverse in inrder order non-recursivly
  inorder_nonrecursive(trees);
  cout<<endl;
}

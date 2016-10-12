#include <stdlib.h>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <list>
using namespace std;

// list node
struct node{
  node*next;
  int val;
};
// make list form vector
node*mklist(vector<int>const&v){
  node dummy{0,0};
  node*l=&dummy;
  for(int val:v){
    l->next=new node{0,val};
    l=l->next;
  }
  return dummy.next;
}
// print list
void printlist(node*l){
  for(;l;l=l->next)cout<<l->val<<" ";
  cout<<endl;
}
// merge two sorted lists
node*merge(node*nl,node*nr){
  node dummy{0,0};
  node*l=&dummy;
  while(nl&&nr){
    if(nl->val<nr->val){
      l->next=nl;
      nl=nl->next;
    }else{
      l->next=nr;
      nr=nr->next;
    }
    l=l->next;
  }
  if(nl)l->next=nl;
  else l->next=nr;
  return dummy.next;
}
// merge sort list
node*msort(node*l){
  if(l==0||l->next==0)return l;

  // split list into two lists
  node*l2=l;
  node*l1=l->next;
  while(l1&&l1->next){
    l2=l2->next;
    l1=l1->next->next;
  }
  node*ltmp=l2;
  l2=l2->next;
  ltmp->next=0;
  l1=l;

  // sort each list
  l1=msort(l1);
  l2=msort(l2);

  // merge lists
  return merge(l1,l2);
}
// kill list
void killist(node*n){
  if(n==0)return;
  killist(n->next);
  delete n;
}
// find maximum element in list recursivly
int maxrec(node*n){
  if(n==0)return 0;
  return max(n->val,maxrec(n->next));
}
// test program
int main(){
  vector<int>v1{0,1,2,3,4,5};
  node*l1=mklist(v1);
  vector<int>v2{0,2,6,7,8};
  node*l2=mklist(v2);
  node*l3=merge(l1,l2);
  printlist(l3);
  vector<int>v4{6,3,4,9,10,22,0,99,12};
  node*l4=mklist(v4);
  printlist(l4);
  node*l4sorted=msort(l4);
  printlist(l4sorted);
  cout<<"max (recursivly): "<<maxrec(l4)<<endl;

  // cleanup
  killist(l3);
  killist(l4sorted);
}

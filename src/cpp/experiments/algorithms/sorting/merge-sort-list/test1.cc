// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.
#include <iostream>
#include <vector>
#include <cassert>
using namespace std;

// node structure
struct Node{
  int val;
  Node*next;
};
// merge two lists
Node*merge(Node*l1,Node*l2){
  Node root=Node{0,nullptr};
  Node*l3=&root;
  while(l1&&l2){
    if(l1->val<l2->val){
      l3->next=l1;
      l3=l1;
      l1=l1->next;
    }else{
      l3->next=l2;
      l3=l2;
      l2=l2->next;
    }
  }
  l3->next=l1?l1:l2;
  return root.next;
}
// sort list
Node*msort(Node*l1){
  if(l1==nullptr||l1->next==nullptr)return l1;
  Node*ltmp=l1;
  Node*ltmpfast=l1->next;
  while(ltmpfast&&ltmpfast->next){
    ltmpfast=ltmpfast->next->next;
    ltmp=ltmp->next;
  }
  Node*l2=ltmp->next;
  ltmp->next=nullptr;
  return merge(msort(l1),msort(l2));
}
// print list
void printlist(Node*l){
  while(l){
    cout<<l->val<<" ";
    l=l->next;
  }
  cout<<endl;
}
// make a list from a vector
Node*makelist(vector<int>const&v){
  Node tmp1{0,nullptr};
  Node*l1=&tmp1;
  for(size_t i=0;i<v.size();++i){
    l1->next=new Node{v[i],nullptr};
    l1=l1->next;
  }
  return tmp1.next;
}
// test program
int main(){
  vector<int>v1{10,3,50,7,9,11,130};
  Node*l1=makelist(v1);
  printlist(l1);
  Node*l2=msort(l1);
  printlist(l2);
}

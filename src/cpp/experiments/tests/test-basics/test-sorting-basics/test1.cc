#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
using namespace std;

int part(vector<int>&v,size_t l,size_t u){       // partition on first element
  size_t m=l;
  for(size_t i=l+1;i<=u;++i){
    if(v[i]<v[l])swap(v[++m],v[i]);
  }
  swap(v[l],v[m]);
  return m;
}
void qsort(vector<int>&v,size_t l,size_t u){      // quick sort
  if(l>=u)return;
  int m=part(v,l,u);
  qsort(v,l,m-1);
  qsort(v,m+1,u);
}
void ssort(vector<int>&v,size_t l,size_t u){      // selection (min) sort
  for(size_t i=l;i<u;++i){
    size_t imin=i;
    for(size_t j=i+1;j<=u;++j){
      if(v[j]<v[imin])imin=j;
    }
    swap(v[i],v[imin]);
  }
}
void isort(vector<int>&v,size_t l,size_t u){      // insertion sort
  for(size_t i=l+1;i<=u;++i){
    for(size_t j=i;j>l;--j){
      if(v[j]<v[j-1])swap(v[j],v[j-1]);
      else break;
    }
  }
}
void merge(vector<int>&v,size_t l,size_t m,size_t u){  // merge sections form same vector
  vector<int>a(u-l+1);
  size_t lind=l,uind=m+1;
  for(size_t i=l;i<=u;++i){               // single loop - not efficient since too many tests but simple
    if(lind>m)a[i-l]=v[uind++];else
    if(uind>u)a[i-l]=v[lind++];
    else a[i-l]=(v[lind]<v[uind])?v[lind++]:v[uind++];
  }
  for(size_t i=l;i<=u;++i)v[i]=a[i-1];
}
void mergebitonic(vector<int>&v,size_t l,size_t m,size_t u){  // use bitonic sequence to merge
  vector<int>a(u-l+1);                           // use bitonic sequence
  for(size_t i=l;i<=m;++i)a[i-l]=v[i];           // copy first sequence in order
  for(size_t i=m+1;i<=u;++i)a[u-i+m+1-l]=v[i];   // copy second sequence in reverse order
  size_t ind1=0,ind2=u-l;                        // merge
  for(size_t i=l;i<=u;++i){
    if(a[ind1]<a[ind2])v[i]=a[ind1++];
    else v[i]=a[ind2--];
  }
}
void msort(vector<int>&v,size_t l,size_t u){      // merge sort (use normal merge or bitonic merge
  if(l>=u)return;
  int m=(l+u)/2;
  msort(v,l,m);
  msort(v,m+1,u);
  merge(v,l,m,u);
//  mergebitonic(v,l,m,u);
}
// heap stuff
size_t hparent(size_t c){
  return (c-1)/2;
}
size_t hchild1(size_t p){
  return (p+1)*2-1;
}
size_t hchild2(size_t p){
  return (p+1)*2;
}
void sinkel(vector<int>&v,size_t n){  // sink top element in heap - #el in heap=n
  size_t p=0;
  while(true){
    if(p>=n)break;
    size_t c1=hchild1(p);
    size_t c2=hchild2(p);
    size_t c=c1;
    if(c>=n)break;
    if(c2<n){
      if(v[c1]<v[c2])c=c1;
      else c=c2;
    }
    if(v[p]<=v[c])return;
    swap(v[c],v[p]);
    p=c;
  }
}
void floatel(vector<int>&v,int c){ // float element 'c' to top
  while(true){
    if(c==0)return;
    size_t p=hparent(c);
    if(v[p]<v[c])return;
    swap(v[p],v[c]);
    c=p;
  }
}
void mkheap(vector<int>&v){        // create a heap from elements in vector [0...n-1]
  size_t n=v.size();
  for(size_t i=0;i<n;++i)floatel(v,i);
}
void hsort(vector<int>&v){         // heap sort
  mkheap(v);
  size_t n=v.size();
  for(size_t i=0;i<n;++i){
    swap(v[0],v[n-i-1]);
    sinkel(v,n-i-1);
  }
}
int main(){
  vector<int>v{5,3,10,18,0,3,20};
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
  cout<<endl;
  hsort(v);
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
  cout<<endl;
}

#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
using namespace std;

// partition
size_t part(vector<int>&v,size_t l,size_t u){
  size_t m=l;
  for(size_t i=l+1;i<=u;++i){
    if(v[i]<v[l])swap(v[++m],v[i]);
  }
  swap(v[m],v[l]);
  return m;
}
// find longest increasing sequence
int longestseq(vector<int>const&v){
  size_t n=v.size();
  size_t sofar=1;        // max len so far
  size_t tilnow=1;       // max len until now
  for(size_t i=1;i<n;++i){
    if(v[i]>v[i-1])++tilnow;
    else tilnow=1;
    if(tilnow>sofar)sofar=tilnow;
  }
  return sofar;
}
// find lowest bound of an integer in a sorted sequence
int lowest(vector<int>const&v,int val){
  int ret=-1;   // default - not found
  int l=0,u=v.size()-1;
  while(true){
    if(l>u)return ret;
    int m=(l+u)/2;
    if(v[m]==val){
      ret=m;               // we found it - continue searching lower half
      u=m-1;
    }else
    if(v[m]>val)u=m-1;     // we are too high - searchg lower half
    else l=m+1;            // we are too low - search higher half
  }
}
// find kth smallest element
int kthsmallest(vector<int>&v,size_t k){
  size_t l=0,u=v.size()-1;
  --k;
  if(k<l||k>u)return -1;
  while(true){
    size_t m=part(v,l,u);
    if(m==k)return v[k];
    if(k<m)u=m-1;
    else l=m+1;
  }
}
// find an element in a sorted sequence
bool find(vector<int>const&v,int val){
  int l=0,u=v.size()-1;
  while(true){
    if(l>u)return false;
    int m=(l+u)/2;
    if(v[m]==val)return true;
    if(v[m]>val)u=m-1;
    else l=m+1;
  }
}
// find longest incrementing sequence
int maxincseq(vector<int>const&v){
  size_t n=v.size();
  int maxsofar=1;
  int max2here=1;
  for(size_t i=1;i<n;++i){
    if(v[i]>v[i-1])++max2here;
    else max2here=1;
    if(max2here>maxsofar)maxsofar=max2here;
  }
  return maxsofar;
}
// find maximum el in array by comparing first element with maximum of rest of array recursivly
int findmaxrec(vector<int>const&v,size_t ind){
  if(ind==v.size()-1)return v[ind];
  int lmax=findmaxrec(v,ind+1);
  if(v[ind]>lmax)return v[ind];
  return lmax;
}
// test program
int main(){
  vector<int>v{10,5,3,10,18,0,3,20,4,22,17};
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
  cout<<endl;
  cout<<"4th smallest: "<<kthsmallest(v,4)<<endl;
  sort(v.begin(),v.end());
  copy(v.begin(),v.end(),ostream_iterator<int>(cout," "));
  cout<<endl;
  cout<<"longest: "<<longestseq(v)<<endl;
  cout<<"find 10: "<<boolalpha<<find(v,10)<<endl;
  cout<<"find 11: "<<boolalpha<<find(v,11)<<endl;
  cout<<"lowest (10): "<<lowest(v,10)<<endl;
  vector<int>v1{10,5,3,10,18,0,3,20,4,22,17};
  copy(begin(v1),end(v1),ostream_iterator<int>(cout," "));
  cout<<endl;
  cout<<"maxincseq: "<<maxincseq(v1)<<endl;
  cout<<"recursive find max: "<<findmaxrec(v,0)<<endl;
}

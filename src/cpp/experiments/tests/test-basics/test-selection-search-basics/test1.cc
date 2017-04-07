#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <tuple>
#include <vector>
#include <climits>
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
  int n=v.size();
  if(n==0)return 0;
  int max2here=1;
  int maxsofar=1;
  for(int i=1;i<n;++i){
    if(v[i]>v[i-1])maxsofar=max(++max2here,maxsofar);
    else max2here=1;
  }
  return maxsofar;
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
// (begin) ---------------- find max sub-array (i.e. sum of array elements) in an array -----------------------------
// find cross-max subarray
// (return <lind,uind,max>)
tuple<int,int,int>crossmax(vector<int>const&v,int l,int m,int u){
  int umaxsofar=-INT_MAX;
  int usum2here=0;
  int uindsofar=-1;
  for(int i=m;i<=u;++i){
    usum2here+=v[i];
    if(usum2here>umaxsofar){
      umaxsofar=usum2here;
      uindsofar=i;
    }
  }
  if(l==m)return make_tuple(m,uindsofar,umaxsofar);
  int lmaxsofar=-INT_MAX;
  int lsum2here=0;
  int lindsofar=-1;
  for(int i=m-1;i>=l;--i){
    lsum2here+=v[i];
    if(lsum2here>lmaxsofar){
      lmaxsofar=lsum2here;
      lindsofar=i;
    }
  }
  if(lmaxsofar<0)return make_tuple(m,uindsofar,umaxsofar);
  return make_tuple(lindsofar,uindsofar,lmaxsofar+umaxsofar);
}
// find max subarray between [l,u]
tuple<int,int,int>totmax(vector<int>const&v,int l,int u){
  if(l>u)return make_tuple(l,u,-INT_MAX);
  if(l==u)return make_tuple(l,u,v[l]);
  int m=(l+u)/2;

  // get max for left, right and cross 
  auto lmax=totmax(v,l,m-1);
  auto rmax=totmax(v,m+1,u);
  auto xmax=crossmax(v,l,m,u);

  // check which one that gave best result
  tuple<int,int,int>maxfound;
  if(get<2>(lmax)>get<2>(rmax)&&get<2>(lmax)>get<2>(xmax)){ // left one greater
    maxfound=lmax;
  }else
  if(get<2>(rmax)>get<2>(lmax)&&get<2>(rmax)>get<2>(xmax)){ // right one greater
    maxfound=rmax;
  }else{
    maxfound=xmax;
  }
  return maxfound;
}
// find max sub-array
decltype(auto)maxsubarray(vector<int>const&v){
  return totmax(v,0,v.size()-1);
}
// (end) ---------------- find max sub-array (i.e. sum of array elements) in an array -----------------------------
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

  cout<<"(begin) --- max-subarray: ";
  vector<int>v2{1,6,-4,-2,6,8,-20,-10,0,4,-2,-2,5,-1,13};
  copy(begin(v2),end(v2),ostream_iterator<int>(cout," "));
  cout<<endl;
  auto res=maxsubarray(v2);
  cout<<"maxsum: "<<get<2>(res)<<", subarray: ";
  for(int i=get<0>(res);i<=get<1>(res);++i){
    cout<<v2[static_cast<size_t>(i)]<<" ";
  }
  cout<<endl;
  cout<<"(end) --- max-subarray: ";
}

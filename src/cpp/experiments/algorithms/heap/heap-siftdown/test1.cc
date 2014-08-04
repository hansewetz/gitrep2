#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>
using namespace std;

// program build a heap, then uses siftdown to write out elements in decending order

vector<int>v;
void siftdown(vector<int>&v){
  int n=v.size()-1;
  int p=1;
  while(true){
    // find smallest child
    int c1=2*p;
    int c2=2*p+1;
    if(c1>n)break;
    int c=c1;
    if(c2<=n){
      if(v[c2]<v[c1])c=c2;
    }
    // check if we should swap parent with smallest child
    if(v[c]<v[p])swap(v[c],v[p]);
    else break;
    p=c;
  }
}
void siftup(vector<int>&v){
  int n=v.size()-1;
  int c=n;
  while(true){
    if(c==0)break;
    int p=c/2;
    if(v[c]<v[p])swap(v[c],v[p]);
    c=p;
  }
}
int main(){
  v.push_back(-1);
  vector<int>vals={17,3,87,34,2,9};
  for_each(vals.begin(),vals.end(),[](int i){v.push_back(i);siftup(v);});
  copy(v.begin()+1,v.end(),ostream_iterator<int>(cout," "));
  cerr<<endl;

  // get smallest elemnts
  while(v.size()>1){
    cerr<<v[1]<<endl;
    v[1]=v[v.size()-1];
    v.pop_back();
    siftdown(v);
  }
}

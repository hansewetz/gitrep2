#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <queue>
#include <map>
#include <set>
#include <typeinfo>
#include <climits>
using namespace std;

// --- calcuate lcs between two vectors
template<typename T>
int lcs(vector<T>const&v1,vector<T>const&v2){
  // create calculation board
  size_t n1=v1.size();
  size_t n2=v2.size();
  vector<vector<int>>m(n1);
  for(size_t i=0;i<n1;++i)m[i]=vector<int>(n2,0);

  // evaluate dp bottom-up
  for(size_t i=1;i<n1;++i){
    for(size_t j=1;j<n2;++j){
      if(v1[i]==v2[j]){
        m[i][j]=m[i-1][j-1]+1;
      }else{
        m[i][j]=max(m[i-1][j],m[i][j-1]);
      }
    }
  }
  return m[n1-1][n2-1];
}
// --- calculate edit distance between two vectors
template<typename T>
int editdist(vector<T>const&v1,vector<T>const&v2){
  // create matrix
  size_t n1=v1.size();
  size_t n2=v2.size();
  vector<vector<int>>K(n1);
  for(size_t i=0;i<n1;++i)K[i]=vector<int>(n2,0);

  // initialize border cases
  for(size_t i=0;i<n1;++i)K[i][0]=i;
  for(size_t j=0;j<n2;++j)K[0][j]=j;

  // do bottom up dp
  for(size_t i=1;i<n1;++i){
    for(size_t j=1;j<n2;++j){
      int d1,d2,d3;
      d1=K[i-1][j-1]+(v1[i]==v2[j]?0:1);
      d2=K[i-1][j]+1;
      d3=K[i][j-1]+1;
      K[i][j]=min(d1,min(d2,d3));
    }
  }
  return K[n1-1][n2-1];
}
// --- evaluate lcs for all combinations against base sentence
template<typename T>
void evallcs(vector<T>const&v,vector<T>&res,vector<bool>&used){
  if(v.size()==res.size()){
    cout<<lcs(v,res)<<":"<<editdist(v,res)<<" --- ";
    copy(begin(res),end(res),ostream_iterator<T>(cout," "));
    cout<<endl;
  }
  for(size_t i=0;i<v.size();++i){
    if(used[i])continue;
    used[i]=true;
    res.push_back(v[i]);
    evallcs(v,res,used);
    res.pop_back();
    used[i]=false;
  }
}
template<typename T>
void evallcs(vector<T>const&v){
  size_t n=v.size();
  vector<bool>used(n,false);
  vector<T>res;
  evallcs(v,res,used);
}
// test program
int main(){
  vector<string>v{"today","the","dog","went","for","a","walk"};
  evallcs(v);
}

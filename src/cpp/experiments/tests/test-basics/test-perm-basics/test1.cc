#include <stdlib.h>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
using namespace std;

// generate all valid combinations of '(' and ')'
// (rule: nl >=nr)
void genparen(int nl,int nr, int maxp,string&res){
  if(nl==maxp&&nr==maxp){
    cout<<res<<endl;
   return;
  }
  int lcount=0;
  for(int l=nl+1;l<=maxp;++l){
    res.push_back('(');
    ++lcount;
    int rcount=0;
    for(int r=nr+1;r<=maxp;++r){
      if(r>l)continue;
      res.push_back(')');
      genparen(l,r,maxp,res);
      ++rcount;
    }
    for(int i=0;i<rcount;++i)res.pop_back();
  }
  for(int i=0;i<lcount;++i)res.pop_back();
}
void genparen(int maxp){
  string res;
  genparen(0,0,maxp,res);
}
// generate all permutation of vector
void genperm(vector<int>const&v,vector<bool>&used,vector<int>&res){
  if(res.size()==v.size()){
    copy(res.begin(),res.end(),ostream_iterator<int>(cout," "));
    cout<<endl;
  }
  for(size_t i=0;i<v.size();++i){
    if(used[i])continue;
    used[i]=true;
    res.push_back(v[i]);
    genperm(v,used,res);
    res.pop_back();
    used[i]=false;
  }
}
void genperm(vector<int>const&v){
  vector<bool>used(v.size(),false);
  vector<int>res;
  genperm(v,used,res);
}
// generate all subsets of a string
void genstrsubsets(string const&s,vector<bool>&use){
  if(use.size()==s.size()){
    for(size_t i=0;i<s.length();++i){
      if(use[i])cout<<s[i];
    }
    cout<<endl;
    return;
  }
  use.push_back(true);
  genstrsubsets(s,use);
  use.pop_back();
  use.push_back(false);
  genstrsubsets(s,use);
  use.pop_back();
}
void genstrsubsets(string const&s){
  vector<bool>use;
  genstrsubsets(s,use);
}
// generate all subsets of a vector
// (loop through all integers and pick out the bits)
void gensubs(vector<int>const&v){
  size_t count=1;
  for(size_t i=0;i<v.size();++i)count*=2;
  for(size_t mask=0;mask<count;++mask){
    size_t ind=0;
    size_t tmp=mask;
    while(tmp){
      if(tmp&1)cout<<v[ind]<<" ";
      ++ind;
      tmp>>=1;
    }
    cout<<endl;
  }
}
// generate all permutation of a string
void markc(char&c){c|=0x80;}
void unmarkc(char&c){c&=~0x80;}
bool cismarked(char c){return c&0x80;}

void genstrperm(string&s,string&res){
  if(res.length()==s.length()){
    cout<<res<<endl;
    return;
  }
  for(size_t i=0;i<s.size();++i){
    if(cismarked(s[i]))continue;
    res.push_back(s[i]);
    markc(s[i]);
    genstrperm(s,res);
    unmarkc(s[i]);
    res.pop_back();
  }
}
void genstrperm(string&s){
  string res;
  genstrperm(s,res);
}
// generate random permutations of a vector
void randperm(vector<int>&v){
  size_t n=v.size();
  for(size_t i=0;i<n-1;++i){
    size_t ind=i+rand()%(n-i); // get random nunber in [i,n-1)
    swap(v[i],v[ind]);
  }
  copy(begin(v),end(v),ostream_iterator<int>(cout," "));
  cout<<endl;
}
// test program
int main(){
  genparen(4);
  genperm(vector<int>{1,2,3,4});
  genstrsubsets("1234");
  gensubs(vector<int>{0,1,2,3,4});
  vector<int>v{0,1,2,3,4};
  randperm(v);
  string pstr="ABCD";
  genstrperm(pstr);
}

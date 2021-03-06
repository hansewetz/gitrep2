#include <stdlib.h>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
using namespace std;

// calculate all valid queen chess positions
bool allowed(vector<int>const&cols,int row,int col){
  for(int r=0;r<row;++r){
    int c=cols[r];
    if(c==col)return false;
    if(abs(r-row)==abs(c-col))return false;
  }
  return true;
}
void q8(vector<int>&cols,int row){
  if(row==8){
    for(int r=0;r<8;++r){
      cout<<"["<<r<<", "<<cols[r]<<"]";
    }
    cout<<endl;
    return;
  }
  for(int c=0;c<8;++c){
    if(!allowed(cols,row,c))continue;
    cols[row]=c;
    q8(cols,row+1);
  }
}
void q8(){
  vector<int>cols(8,-1);
  q8(cols,0);
}
// (1) generate all valid combinations of '(' and ')'
void genp(int nl,int nr,int maxp,string res){  // rule: nl>=nr
  if(nl<nr||nl>maxp)return; // invalid comb. of right/left @of paren.
  if(nl==maxp&&nr==maxp){
    cout<<res<<endl;
    return;
  }
  res.push_back('(');
  genp(nl+1,nr,maxp,res);
  res.pop_back();
  res.push_back(')');
  genp(nl,nr+1,maxp,res);
  res.pop_back();
}
void genp(int np){
  string res;
  genp(0,0,np,res);
}
// (2) generate all valid combinations of '(' and ')'
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
// generate all paths from position [0, 0] --> position [N, M] in a grid
constexpr int nrows=2,ncols=3;
void path(bool used[nrows][ncols],int row,int col,vector<pair<int,int>>&v){
  if(row==nrows-1&&col==ncols-1){
    for(size_t i=0;i<v.size();++i){
      cout<<"["<<v[i].first<<", "<<v[i].second<<"]";
    }
    cout<<endl;
    return;
  }
  // try walking down
  if(row!=nrows-1){
    if(!used[row+1][col]){
      used[row+1][col]=true;
      v.push_back(make_pair(row+1,col));
      path(used,row+1,col,v);
      v.pop_back();
      used[row+1][col]=false;
    }
  }
  if(col!=ncols-1){
    if(!used[row][col+1]){
      used[row][col+1]=true;
      v.push_back(make_pair(row,col+1));
      path(used,row,col+1,v);
      v.pop_back();
      used[row][col+1]=false;
    }
  }
}
void genpaths(){
  bool used[nrows][ncols];
  for(int i=0;i<nrows;++i){
    for(int j=0;j<ncols;++j){
      used[i][j]=false;
    }
  }
  vector<pair<int,int>>v;
  path(used,0,0,v);
}
// test program
int main(){
  genp(4);
  //genparen(4);
  genperm(vector<int>{1,2,3,4});
  genstrsubsets("1234");
  gensubs(vector<int>{0,1,2,3,4});
  vector<int>v{0,1,2,3,4};
  randperm(v);
  string pstr="ABCD";
  genstrperm(pstr);
}

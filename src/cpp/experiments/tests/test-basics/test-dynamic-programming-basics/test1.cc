#include <stdlib.h>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <map>
#include <climits>
using namespace std;

// item to pack
struct item_t{
  int size;
  int value;
};
// calculate max value for a sack of size: space
int knap(int space,vector<item_t>items,map<int,int>&space2value){
  // for each type of item recurse to see what max we can get
  int maxval=0;
  for(auto const&item:items){
    // get space left if we take this item - if it can't fit then continue
    int spaceleft=space-item.size;
    if(spaceleft<0)continue;

    // check if we already have value for this space
    auto it=space2value.find(space);
    if(it!=space2value.end()){
      maxval=max(maxval,it->second);
    }else{
      int tmpmaxval=knap(spaceleft,items,space2value)+item.value;
      maxval=max(maxval,tmpmaxval);
      space2value[space]=maxval;
    }
  }
  return maxval;
}
// longest common sequence
// (dynamic programming)
int lcs(vector<int>const&v1,vector<int>const&v2){
  auto n1=v1.size();
  auto n2=v2.size();

  int tab[100][100];
  for(size_t i=0;i<n1;++i){
    for(size_t j=0;j<n2;++j){
      tab[i][j]=0;
    }
  }
  for(size_t i=1;i<n1;++i){
    for(size_t j=1;j<n2;++j){
      if(v1[i]==v2[j]){
        tab[i][j]=1+tab[i-1][j-1];
      }else{
        tab[i][j]=max(tab[i-1][j],tab[i][j-1]);
      }
    }
  }
  return tab[n1-1][n2-1];
}
/*
  - rod cutting (maximize profit given a table of unit-price for rod length):
        K[n]=max{K[n-i]+pricePerUnit[i]*i; i=1..n}
             0;             ; n==0
*/
// returns maximum value
// (do bottom up approach)
int cutrod(int length,vector<int>const&pricePerUnit,vector<int>&cutpoint,vector<int>&res){
  res[0]=0;
  for(int n=1;n<=length;++n){
    for(int i=1;i<=n;++i){
      int currprofit=res[n-i]+pricePerUnit[i]*i;
      if(res[n]<currprofit){
        res[n]=currprofit;
        cutpoint[n]=i;
      }
    }
  }
  return res[length];
}
// print rods
void printrods(int length,vector<int>cutpoint,vector<int>&res){
  if(length==0){
    cout<<endl;
    return;
  }
  cout<<cutpoint[length]<<" ";
  printrods(length-cutpoint[length],cutpoint,res);
}
void rodcutTest(){
  int length=7;
  vector<int>pricePerUnit{0,0,2,3,2,2,1,1};
  vector<int>res(length+1,-INT_MAX);
  vector<int>cutpoint(length+1,-INT_MAX);
  int maxprofit=cutrod(length,pricePerUnit,cutpoint,res);
  cout<<"maxprofit: "<<maxprofit<<endl;
  cout<<"cutlengths: ";
  printrods(length,cutpoint,res);
}
// find lowest ind in v where v[ind]>=val
// (we know ind is in [1..n])
int lisFindCeil(vector<int>const&v,int val){
  int l=1,u=v.size()-1;
  while(l<u){
    int m=(l+u)/2;
    if(val==v[m])return m;
    if(val<v[m]){
      u=m-1;
    }else{
      l=m+1;
    }
  }
  return u;
}
// longest increasing sub-sequence
int lis(vector<int>const&v){
  if(v.size()<2)return v.size();
  vector<int>len2elmap{0,v[0]};    // len2elmap[len]==<last-element>
  int maxlen=1;                    // longest seq so far
  for(size_t i=maxlen;i<v.size();++i){
    if(v[i]>len2elmap[maxlen]){        // new lis
      len2elmap.push_back(v[i]);
      ++maxlen;
      continue;
    }
    if(v[i]<v[1]){             // shortest lis must now end with lower number
      len2elmap[1]=v[i];
      continue;
    }
    // find list having lowest end element greater than v[i]
    int ind=lisFindCeil(len2elmap,v[i]);
    len2elmap[ind]=v[i];
  }
  return maxlen;
}
// test program
int main(){
  vector<item_t>items{{300,10},{200,20},{500,50},{600,20}};
  map<int,int>space2valuemap;
  int space=1000;
  cout<<"--- knapsack ----"<<endl;
  cout<<"size: "<<space<<": "<<knap(space,items,space2valuemap)<<endl;

  // dump space->value map
  for(auto const&p:space2valuemap){
    cout<<"[space, value]: ["<<p.first<<", "<<p.second<<"]"<<endl;
  }
  cout<<"--- LIS (longest increasing sub-sequence -----"<<endl;
  vector<int>v{1,2,3,4,5,2,3,4,5,1,2,3,7,8,9,2,10};
  cout<<lis(v)<<endl;

  cout<<"--- LCS (longest common sequence --------------"<<endl;
  vector<int>v1{1,3,4,5,7,10};
  vector<int>v2{3,4,5,6,7};
  cout<<lcs(v1,v2)<<endl;

  // max profit cutting a rod into pieces with a price table for unit price
  cout<<"----- cut a rod into pieces with a price table for unit price ------------"<<endl;
  rodcutTest();
}

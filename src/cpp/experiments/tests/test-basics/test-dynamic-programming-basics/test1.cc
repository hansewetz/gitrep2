#include <stdlib.h>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <map>
#include <climits>
using namespace std;

// reverese graph
using graph_t=vector<vector<int>>;
constexpr int MAXINT=1000;

graph_t rgraph(graph_t const&g){
  int n=g.size();
  graph_t rg=graph_t(n,vector<int>());
  for(int u=0;u<n;++u){
    for(auto v:g[u]){
      rg[v].push_back(u);
    }
  }
  return rg;
}
// shortest path from 'start' --> 'v' of max 'k' length
int shortest_k_path(graph_t const&g,int start,int v,int k,map<pair<int,int>,int>&res){
  if(res.count(pair(v,k))){
    return res[pair(v,k)];
  }
  if(start==v){
    res[pair(v,k)]=0;
    return 0;
  }
  if(k==0){
    res[pair(v,k)]=MAXINT;
    return MAXINT;
  }
  int minlen=MAXINT;
  for(int u:g[v]){
    int len=shortest_k_path(g,start,u,k-1,res)+1;
    minlen=min(minlen,len);
  }
  res[pair(v,k)]=minlen;
  return minlen;
}
int shortest_k_path(graph_t const&g,int start,int end,int k){
  graph_t rg=rgraph(g);
  map<pair<int,int>,int>res;
  return shortest_k_path(rg,start,end,k,res);
}
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
  size_t n1=v1.size();
  size_t n2=v2.size();
  vector<vector<int>>tab(n1+1,vector<int>(n2+1,0));
  for(size_t i=1;i<=n1;++i){
    for(size_t j=1;j<=n2;++j){
      if(v1[i-1]==v2[j-1])tab[i][j]=tab[i-1][j-1]+1;
      else tab[i][j]=max(tab[i-1][j],tab[i][j-1]);
    }
  }
  return tab[n1][n2];
}
/*
  - rod cutting (maximize profit given a table of unit-price for rod length):
        K[n]=max{K[n-i]+pricePerUnit[i]*i; i=1..n}
             0;             ; n==0
*/
// returns maximum value
// (do bottom up approach)
int cutrod_rec(int length,vector<int>const&pricePerUnit,vector<int>&cutpoint,vector<int>&res){
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
  int maxprofit=cutrod_rec(length,pricePerUnit,cutpoint,res);
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
// recursive rod cutting
int maxrodprice_rec(int n,vector<int>const&ppu,vector<int>&tab){
  if(n==0)return 0;
  if(tab[n]>=0)return tab[n];
  int pmax=-1;
  for(int i=1;i<=n;++i){
    pmax=max(pmax,maxrodprice_rec(n-i,ppu,tab)+i*ppu[i]);
  }
  tab[n]=pmax;
  return pmax;
}
int maxrodprice_rec(int n,vector<int>const&ppu){
  vector<int>tab(n+1,-1);
  return maxrodprice_rec(n,ppu,tab);
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
// coin change count
int cchange(int a,int k,vector<int>const&denom,map<pair<int,int>,int>&res){
  // check boundary conditions
  if(a<0)return 0;
  if(k<0)return 0;
  if(a==0)return 1;

  // check if we already have it
  if(res.count(pair(a,k)))return res[pair(a,k)];
  
  int ret=cchange(a,k-1,denom,res)+cchange(a-denom[k],k,denom,res);
  res[pair(a,k)]=ret;
  return ret;
}     
int cchange(int a,vector<int>const&denom){
  map<pair<int,int>,int>res;
  return cchange(a,denom.size()-1,denom,res);
}  
// find implement longest sub-sequence in x which is a substring in y
// nx --> length of seq x
// ny --> length of seq y
// N(i,j)=0 if i==0 or j==0
//       =N[i-1,j-1] if x(i)==y(j)
//       =N[i-1, j] otherwise because we want a substring in y
// result is the maximum of N[nx,j]; 0<=j<=ny
//
int longss(vector<int>const&x,vector<int>const&y,int nx,int ny,vector<vector<int>>&tab){
  if(tab[nx][ny]>=0)return tab[nx][ny];
  int ret=0;
  if(x[nx-1]==y[ny-1])ret=longss(x,y,nx-1,ny-1,tab)+1;
  else ret=longss(x,y,nx-1,ny,tab);
  tab[nx][ny]=ret;
  return ret;
}
int longss(vector<int>const&x,vector<int>const&y){
  int nx=x.size();
  int ny=y.size();
  vector<vector<int>>tab(nx+1,vector<int>(ny+1,-INT_MAX));
  for(int i=0;i<=nx;++i)tab[i][0]=0;
  for(int j=0;j<=ny;++j)tab[0][j]=0;
  longss(x,y,nx,ny,tab);
  int ret=-INT_MAX;
  for(int j=0;j<=ny;++j)ret=max(ret,tab[nx][j]);
  return ret;
}
// find sum of increasing sub-sequence of length k which has minimum sum
// (1 based array in explanation)
// S(n,k) =   # n --> sub-seq ending at n, k --> length of sub-sequence
//            -1; if: n<=0 or k<=0
//            min(v[i]); 1<=i<=n; if: k==1
//            min(
//                (1) {S(i,k-1)+v[n]; if v[n]<=v[i]; 1<=i<n} else MAX};
//                (2) S(i,k)); 1<=i<n;
//            )
int minss(vector<int>const&v,int n,int k,vector<vector<int>>&tab){
  if(k<=0||n<=0)return 1000;
  if(tab[n][k]!=1000)return tab[n][k];
  if(k==1){
    int ret=1000;
    for(int i=1;i<=n;++i)ret=min(ret,v[i-1]);
    tab[n][k]=ret;
    return ret;
  }
  int ret=1000;
  for(int i=1;i<n;++i){
    if(v[i-1]<v[n-1]){
      int res1=minss(v,i,k-1,tab)+v[n-1];
      int res2=minss(v,i,k,tab);
      ret=min(ret,min(res1,res2));
    }
  }
  tab[n][k]=ret;
  return ret;
}
int minss(vector<int>const&v,int k){
  int n=v.size();
  vector<vector<int>>tab(n+1,vector<int>(k+1,1000));
 return minss(v,n,k,tab);
}
// knspasack with no duplicates
int knap_nodup(int volume,int k,vector<int>const&sizes,vector<int>const&values,map<pair<int,int>,int>&dp,map<pair<int,int>,bool>&picked){
  if(k==0)return 0;
  if(volume<=0)return 0;
  if(map<pair<int,int>,int>::iterator it=dp.find(pair(volume,k)); it!=dp.end()){
    return it->second; 
  }
  auto res1=knap_nodup(volume-sizes[k-1],k-1,sizes,values,dp,picked)+values[k-1];
  auto res2=knap_nodup(volume,k-1,sizes,values,dp,picked);

  // keep track ofif item was chosen
  if(res1>res2)picked[pair(volume,k-1)]=true;
  else picked[pair(volume,k-1)]=false;

  auto resmax=max(res1,res2);
  dp[pair(volume,k)]=resmax;
  return resmax;
}
int knap_nodup(int volume,vector<int>const&sizes,vector<int>const&values,map<pair<int,int>,bool>&picked){
  map<pair<int,int>,int>dp;
  return knap_nodup(volume,values.size(),sizes,values,dp,picked);
}
void printItems(int volume,vector<int>const&sizes,map<pair<int,int>,bool>const&picked){
  cout<<boolalpha;
  int k=sizes.size();
  while(volume>0){
    if(picked.count(pair(volume,k-1))){
      cout<<k<<" ";
      volume-=sizes[k-1];
    }
    --k;
  }
  cout<<endl;
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

  // count #of ways to give change on an amount having a set of coins
  cout<<"----- count #of ways to give change on an amount ------------------"<<endl;
  vector<int>coins{1,5,10,25,50,100};
  int amount=10;
  cout<<"#ways to give change: "<<cchange(amount,coins)<<endl;

  cout<<"----- rod cutting - recursive ---------------------------------"<<endl;
  vector<int>ppu{0,5,4,3,2,1}; // price per unit, index == length of rod
  int rodlen=5;
  cout<<"(4) rodprice-rec: "<<maxrodprice_rec(rodlen,ppu)<<endl;

  // find longest sub-sequence in x which is a substring in y
  cout<<"----- find longest sub-sequence in x which is a substring in y"<<endl;
  vector<int>x{1,4,6,2,9,6,3};
  vector<int>y{0,1,2,3};
  cout<<"(2) longss(x,y): "<<longss(x,y)<<endl;

  // find sum non-decreasing sub-sequence of length k which has minimum sum
  cout<<"----- find sum non-decreasing sub-sequence of length k which has minimum sum"<<endl;
  vector<int>vmss{5,2,4,1,9,5};
  int kmss=3;
  int resmss=minss(vmss,kmss);
  cout<<"(3) sum of non-decreasing sub-sequence of length "<<kmss<<": "<<resmss<<endl;

  // find shortest path from 'start' --> 'emd' using max 'k' edges
  cout<<"----- find shortest path from 'start' --> 'emd' using max 'k' edges"<<endl;
  graph_t g{
    {1,2},
    {3},
    {4},
    {4},
    {},
  };
  int start=0;
  int end=4;
  int k=2;
  int lmin=shortest_k_path(g,start,end,k);
  cout<<"(4) lmin: "<<lmin<<endl;

  // knap with no duplicates
  vector<int>sizes{3,5,2,8};
  vector<int>values{100,200,300,400};
  map<pair<int,int>,bool>picked;
  int volume=10;
  cout<<"value: "<<knap_nodup(volume,sizes,values,picked)<<endl;

  // print the items we picked
  printItems(volume,sizes,picked);
}

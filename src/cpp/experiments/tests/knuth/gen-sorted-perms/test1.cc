// Knuth: Alg. L, section 7.2.1.2, Volume 4A
// generate all permutations from a sorted list - do not generate duplicate permutation objects
// (slighly simplified compare dto dscription in Knuth)

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
using namespace std;

// print perm
void print_perm(vector<int> const&v){
  copy(v.begin()+1,v.end(),ostream_iterator<int>(cout," "));
  cout<<endl;
}
// print all permutations (don't generate duplicate objects)
// (starting from index 1)
void gen_perm(vector<int>&v){
  int n=v.size()-1;
  if(n==1)print_perm(v);

  while(true){
    // visit perm object
    print_perm(v);

    // find largest j such that v[j] < v[j+1]
    int j=n-1;
    while(v[j]>=v[j+1]){
      if(--j==0)return;
    }
    // find largest l greater than j such that v[j] < v[l]
    int l=n;
    while(v[j]>=v[l])--l;

    // swap values
    swap(v[j],v[l]);
  
    //  ...
    int k=j+1;
    l=n;
    while(k<l){
      swap(v[k],v[l]);
      ++k;
      --l;
    }
  }
}
int main(){
  vector<int>v{0,1,2,2,3};
  gen_perm(v);
}

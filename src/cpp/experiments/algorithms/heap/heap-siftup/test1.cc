#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <random>
using namespace std;

vector<int> v;
void siftup(){
  int n=v.size()-1;
  int c=n;
  while(true){
    int p=c/2;
    if(p==0)break;
    if(v[c]<v[p])swap(v[p],v[c]);
    c=p;
  }
}
int main(){
  v.push_back(0); // dummy
  vector<int>vals={5,76,4,6,4,3,9,5,6,3,5};
  for_each(vals.begin(),vals.end(),[&](int i){v.push_back(i);siftup();});
  copy(v.begin()+1,v.end(),ostream_iterator<int>(cout," "));
}

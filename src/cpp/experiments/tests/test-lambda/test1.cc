#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
using namespace std;

// main.
int main(int argc,char**argv){
  function<void(int)>f([](int i){cerr<<i<<endl;});
  vector<int>v{3,1,8,5,3,4,9,7};
  sort(v.begin(),v.end());
  for(auto i:v)f(i);
}


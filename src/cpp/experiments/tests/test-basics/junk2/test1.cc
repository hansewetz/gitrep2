#include <stdlib.h>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <map>
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
    auto it=space2value.find(spaceleft);
    if(it!=space2value.end()){
      maxval=max(maxval,it->second);
    }else{
      int tmpmaxval=knap(spaceleft,items,space2value)+item.value;
      maxval=max(maxval,tmpmaxval);
      space2value[spaceleft]=maxval;
    }
  }
  return maxval;
}

// test program
int main(){
  vector<item_t>items{{3,10},{2,20},{5,50},{6,20}};
  map<int,int>tmpmap;
  int space=1000;
  cout<<"size: "<<space<<": "<<knap(space,items,tmpmap)<<endl;
}

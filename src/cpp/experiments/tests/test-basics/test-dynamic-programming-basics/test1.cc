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
// test program
int main(){
  vector<item_t>items{{300,10},{200,20},{500,50},{600,20}};
  map<int,int>space2valuemap;
  int space=1000;
  cout<<"size: "<<space<<": "<<knap(space,items,space2valuemap)<<endl;

  // dump space->value map
  for(auto const&p:space2valuemap){
    cout<<"[space, value]: ["<<p.first<<", "<<p.second<<"]"<<endl;
  }
}

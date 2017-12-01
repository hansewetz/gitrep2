#include "json.hpp"
#include <fstream>
#include <iostream>
using namespace std;

// test program
int main(){
  ifstream is("addr.json");
  nlohmann::json js;
  is>>js;
  cout<<js.dump()<<endl;
}

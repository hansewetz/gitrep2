// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "unicode.hpp"
#include<map>
#include<algorithm>
#include<numeric>
#include<sstream>
using namespace std;
using namespace b2;

// Main test program.
int main(){
  // create stream iterators iterating
  istreambuf_iterator<char>sstr_it_begin(cin);
  istreambuf_iterator<char>sstr_it_end;

  // create utf8 iterators
  auto uit_begin(make_unicode_input_iterator<utf8_tag>(sstr_it_begin));
  auto uit_end(make_unicode_input_iterator<utf8_tag>(sstr_it_end));

  // count #of code quotation code points
  map<cp_t,size_t>count;
  for(auto it=uit_begin;it!=uit_end;++it)count[*it]++;

  // print out quote counts
  for(auto const&p:count){
    cout<<"cp: 0x"<<hex<<p.first<<", count: "<<dec<<p.second<<endl;
  }
}

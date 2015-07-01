// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "unicode.hpp"
#include<map>
#include<iterator>
#include<iostream>
using namespace std;
using namespace b2;

// Main test program.
int main(){
  // create stream iterators reading from stdin
  istream_iterator<char>begin{cin};
  istream_iterator<char>end;

  // create utf8 iterators from input stream iterators
  auto it8_begin(make_unicode_input_iterator<utf8_tag>(begin));
  auto it8_end(make_unicode_input_iterator<utf8_tag>(end));

  // save count for each unicode code point
  map<cp_t,size_t>count;
  for(auto it=it8_begin;it!=it8_end;++it)count[*it]++;

  // print unicode code point together with corresponding count
  for(auto const&p:count){
    cout<<"cp: 0x"<<hex<<p.first<<", count: "<<dec<<p.second<<endl;
  }
}

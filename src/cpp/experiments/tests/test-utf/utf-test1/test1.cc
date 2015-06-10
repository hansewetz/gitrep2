// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "../utf-utils/unicode.hpp"
#include<map>
#include<vector>
#include<algorithm>
#include<numeric>
#include<sstream>

// Main test program.
int main(){
  // setup begin/end iterators for a string in UTF8 encoded format
  std::stringstream sstr("\x68\x65\x6c\x6c\x6f\x20\x65\xcc\x81\x20\xf0\xaf\xa8\x9d\x20\x77\x6f\x72\x6c\x64");
  std::istreambuf_iterator<char>sstr_it_begin(sstr.rdbuf());
  std::istreambuf_iterator<char>sstr_it_end;

  // create utf8 iterators
  b2::unicode_input_iterator<b2::utf8_tag,std::istreambuf_iterator<char> >uit_begin(b2::make_unicode_input_iterator<b2::utf8_tag>(sstr_it_begin));
  b2::unicode_input_iterator<b2::utf8_tag,std::istreambuf_iterator<char> >uit_end(b2::make_unicode_input_iterator<b2::utf8_tag>(sstr_it_end));

  // code points for quotation marks (UNICODE in hex)
  std::vector<b2::cp_t>quotes{0x2FA1D};

  // count #of code quotation code points
  std::map<b2::cp_t,std::size_t>count;
  for(auto it=uit_begin;it!=uit_end;++it){
    for(auto cp:quotes){
      if(cp==*it){
        count[cp]++;
        break;
      }
    }
  }
  // print out quote counts
  for(auto const&p:count){
    std::cout<<"cp: "<<std::hex<<p.first<<", count: "<<std::dec<<p.second<<std::endl;
  }
}

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "unicode.hpp"
#include<list>
#include<algorithm>
#include<numeric>
#include<sstream>
#include<boost/bind.hpp>
#include<boost/iterator/reverse_iterator.hpp>
#include<boost/container/vector.hpp>
#include<boost/range/algorithm.hpp>
#include<boost/range/as_literal.hpp>

// Print a code point in hex.
template<typename T>
struct to_hex:std::unary_function<T,void>{
  to_hex(std::ostream&os):os_(os){}
  void operator()(T cp){os_<<std::hex<<cp<<" ";}
  std::ostream&os_;
};
// Dump a range in hex.
template<typename InputIt>
void hex_dump(std::string const&header,InputIt begin,InputIt end){
  typedef typename std::iterator_traits<InputIt>::value_type value_type;
  std::cerr<<header<<std::endl;
  std::for_each(begin,end,to_hex<value_type>(std::cout));
  std::cerr<<std::endl;
}
// Get max len encoding for a code point.
template<typename Tag>struct get_max_encode_len;
template<>struct get_max_encode_len<b2::utf8_tag>{
  get_max_encode_len():max_(0){}
  void operator()(b2::cp_t cp){max_=std::max(max_,encode_len(cp));}
  size_t max_;
  b2::unicode_cp_encode_length<b2::utf8_tag>encode_len;
};

// Main test program.
int main(){
  // Setup a container of utf8 encoded code points.
  typedef unsigned char char_t;
  char_t valid_utf8_seq[]={0xE2,0x82,0xAC,0xF0,0xA4,0xAD,0xA2,0xC2,0xA2,0x55};
  typedef boost::container::vector<char_t>cont_t;
  typedef boost::container::vector<char_t>::iterator iter_t;
  typedef b2::const_unicode_iterator<b2::utf8_tag,cont_t::iterator>unicode_const_iter;

  cont_t c(valid_utf8_seq,valid_utf8_seq+sizeof(valid_utf8_seq)/sizeof(valid_utf8_seq[0]));
  unicode_const_iter ustart(c.begin());
  unicode_const_iter uend(c.end());

  // Validate encoding.
  b2::uni_error uerr;
  iter_t err_it=b2::unicode_validate_encoding<b2::utf8_tag>(c.begin(),c.end(),&uerr);
  if(err_it!=c.end()){
    std::cerr<<"failed validation, error at octet: "<<std::hex<<static_cast<unsigned short>(*err_it)<<", error: "<<uerr<<std::endl;
    exit(1);
  }
  else{
    std::cerr<<"passed validation"<<std::endl;
  }
  // --- All (most) operations should be safe on the range of utf8 encoded code points.

  // Print each code point from an utf8 encoding.
  std::cout<<"cps (normal order):";
  std::for_each(ustart,uend,to_hex<b2::cp_t>(std::cout));
  std::cout<<std::endl;

  // Print each code point from end to start.
  boost::reverse_iterator<unicode_const_iter>urstart(uend);
  boost::reverse_iterator<unicode_const_iter>urend(ustart);
  std::cout<<"cps (reverse direction):";
  std::for_each(urstart,urend,to_hex<b2::cp_t>(std::cout));
  std::cout<<std::endl;

  // Find a code point.
  unicode_const_iter it=std::find(ustart,uend,static_cast<b2::cp_t>(0x24b62));
  if(it!=uend)std::cout<<"found code point: "<<"0x"<<std::hex<<*it<<std::endl;
  else std::cout<<"did not find code point"<<std::endl;

  // Find first non-ascii code point.
  it=find_if(ustart,uend,boost::bind(std::less<b2::cp_t>(),_1,static_cast<b2::cp_t>(128)));
  if(it!=uend){
    std::cerr<<"found ascii: "<<std::hex<<"0x"<<*it<<std::endl;
  }
  // Print length of all code units.
  for(iter_t it=c.begin();it!=c.end();){
    size_t len=b2::unicode_cu_encode_length<b2::utf8_tag>()(*it);
    std::cerr<<"Length of first code point: "<<len<<std::endl;
    std::advance(it,len);
  }
  // Get max/min code point.
  std::cerr<<"max cp: "<<std::hex<<"0x"<<*std::max_element(ustart,uend)<<", min cp: "<<"0x"<<*std::min_element(ustart,uend)<<std::endl;

  // Count code points.
  std::cerr<<std::dec<<"#codepoints using count_if: "<<std::count_if(ustart,uend,b2::unicode_cp_is_valid())<<std::endl;

  // Count code points using distance.
  std::cerr<<"#codepoints using distance: "<<std::distance(ustart,uend)<<std::endl;

  // Count code points bumping iterator.
  std::iterator_traits<unicode_const_iter>::difference_type count=0;
  for(unicode_const_iter it=ustart;it!=uend;++it)++count;
  std::cerr<<"#codepoints using iterator: "<<count<<std::endl;

  // Calculate length to encode c.begin() ... c.end().
  b2::unicode_cp_encode_length<b2::utf8_tag>encode_len;
  size_t total_len=std::accumulate(ustart,uend,0,boost::bind(std::plus<size_t>(),boost::bind(encode_len,_2),_1));
  std::cerr<<"Total encode length: "<<std::dec<<total_len<<std::endl;

  // Get max len encoding for a code point.
  get_max_encode_len<b2::utf8_tag>max_encode_len;
  max_encode_len=std::for_each(ustart,uend,max_encode_len);
  std::cout<<"Max encode length: "<<max_encode_len.max_<<std::endl;

  // Convert cps to cus and back again.
  boost::container::vector<b2::cp_t>cps;
  std::copy(ustart,uend,std::back_inserter(cps));
  boost::container::vector<char_t>cus;
  std::for_each(cps.begin(),cps.end(),b2::make_unicode_encode<b2::utf8_tag>(std::back_inserter(cus)));
  std::cerr<<"cu len: "<<cus.size()<<std::endl;
  // Convert back to cps.
  unicode_const_iter u1start(cus.begin());
  unicode_const_iter u1end(cus.end());
  boost::container::vector<b2::cp_t>().swap(cps);
  std::copy(u1start,u1end,back_inserter(cps));
  std::cout<<std::hex;
  copy(cps.begin(),cps.end(),std::ostream_iterator<b2::cp_t>(std::cout," "));
  std::cout<<std::endl;

  // Test unicode container.
  try{
    // Construct using codeunit iterators.
    b2::utf8string ustring1(b2::utf8_tag(),c.begin(),c.end());

    // Construct from container holding code units.
    b2::utf8string ustring2(b2::utf8_tag(),c);

    // Construct using cp iterators.
    b2::utf8string ustring3(ustart,uend);

    // Construct using copy constructor.
    b2::utf8string ustring4(ustring1);

    // Construct using zero terminated sequence of code points (expressed as wchar_t).
    b2::utf8string ustring5(L"hello e\u0301 \U0002FA1D world");

    // Construct using default ctor, then assign.
    b2::utf8string ustring6;
    ustring6=ustring1;

    // Construct from container holding codepoints, using boost::iterator_range
    // (NOTE! Should implement direct support for iterator ranges in b2::unicode_container.x)
    const wchar_t wstr7_[]=L"hello e\u0301 \U0002FA1D world";
    boost::iterator_range<const wchar_t*>wstr7=boost::as_literal(wstr7_);
    b2::utf8string ustring7(wstr7.begin(),wstr7.end());

    // Print codepoints in container.
    std::cout<<"ustring5:"<<std::endl;
    for(b2::utf8string::const_iterator it=ustring5.cbegin();it!=ustring5.cend();++it){
      std::cout<<"0x"<<std::hex<<*it<<" ";
    }
    std::cerr<<std::endl;

    // Print container in reverse order.
    std::cout<<"ustring5 (reverse order):"<<std::endl;
    for(b2::utf8string::const_reverse_iterator it=ustring5.crbegin();it!=ustring5.crend();++it){
      std::cout<<"0x"<<std::hex<<*it<<" ";
    }
    std::cerr<<std::endl;

    // Print underling container content.
    std::cout<<"ustring5 underlying container (encoding):"<<std::endl;
    typedef b2::utf8string::container_type encoded_container;
    encoded_container const&cencode(ustring5.base_cont());
    for(encoded_container::const_iterator it=cencode.begin();it!=cencode.end();++it){
      std::cout<<"0x"<<std::hex<<static_cast<unsigned short>(*it)<<" ";
    }
    // Print number of cps in container.
    std::cerr<<"size: "<<ustring1.size()<<std::endl;

    // Test creating an utf8string from streambuf iterators.
    std::cout<<"ustring8 from streambuf iterators:"<<std::endl;
    std::stringstream sstr("\x68\x65\x6c\x6c\x6f\x20\x65\xcc\x81\x20\xf0\xaf\xa8\x9d\x20\x77\x6f\x72\x6c\x64");
    std::istreambuf_iterator<char>sstr_it_begin(sstr.rdbuf());
    std::istreambuf_iterator<char>sstr_it_end;
    b2::utf8string ustring8(b2::utf8_tag(),sstr_it_begin,sstr_it_end);
    for(b2::utf8string::const_iterator it=ustring8.cbegin();it!=ustring8.cend();++it){
      std::cout<<"0x"<<std::hex<<*it<<" ";
    }
    std::cerr<<std::endl;
  }
  catch(std::exception&e){
    std::cerr<<"caught exception while constructing unicode container: "<<e.what()<<std::endl;
  }
  // Test unicode output iterator.
  std::cerr<<"output iterator test (converting cps to cus for utf8: "<<std::endl;
  wchar_t sstr1[]=L"hello e\u0301 \U0002FA1D world";
  boost::iterator_range<const wchar_t*>isstr=boost::as_literal(sstr1);
  boost::container::vector<b2::cp_t>cps1(isstr.begin(),isstr.end());
  boost::container::vector<char_t>cus1;
  std::copy(cps1.begin(),cps1.end(),b2::make_unicode_output_iterator<b2::utf8_tag>(std::back_inserter(cus1)));
  for(boost::container::vector<char_t>::const_iterator it=cus1.begin();it!=cus1.end();++it){
    std::cerr<<std::hex<<"0x"<<static_cast<unsigned short>(*it)<<" ";
  }
  std::cerr<<std::endl;

  // Test input iterator (reading cus and returning cps).
  std::cout<<"Use of input iterators to convert from cus to cps:"<<std::endl;
  std::stringstream sstr("\x68\x65\x6c\x6c\x6f\x20\x65\xcc\x81\x20\xf0\xaf\xa8\x9d\x20\x77\x6f\x72\x6c\x64");
  std::istreambuf_iterator<char>sstr_it_begin(sstr.rdbuf());
  std::istreambuf_iterator<char>sstr_it_end;
  b2::unicode_input_iterator<b2::utf8_tag,std::istreambuf_iterator<char> >uit_begin(b2::make_unicode_input_iterator<b2::utf8_tag>(sstr_it_begin));
  b2::unicode_input_iterator<b2::utf8_tag,std::istreambuf_iterator<char> >uit_end(b2::make_unicode_input_iterator<b2::utf8_tag>(sstr_it_end));
  std::for_each(uit_begin,uit_end,to_hex<b2::cp_t>(std::cout));
  std::cout<<std::endl;
  
  // Test input iterator with validation.
  std::cout<<"Use of input iterators to validate cus:"<<std::endl;
  std::stringstream sstr2("\x68\x65\x6c\x6c\x6f\x20\x65\xcc\x81\x20\xf0\xaf\xa8\x9d\x20\x77\x6f\x72\x6c\x64");
  std::istreambuf_iterator<char>sstr_it_begin2(sstr2.rdbuf());
  std::istreambuf_iterator<char>sstr_it_end2;
  std::istreambuf_iterator<char>it_res2=b2::unicode_validate_encoding<b2::utf8_tag>(sstr_it_begin2,sstr_it_end2,&uerr);
  if(it_res2==sstr_it_end2){
    std::cout<<"validation of stream iterator succeeded"<<std::endl;
  }else{
    std::cout<<"validation of stream iterator failed, error: "<<uerr<<std::endl;
  }
  // Get allocator for a container.
  std::cout<<"Constructing an utf8string from null terminated cps"<<std::endl;
  b2::utf8string s8(L"hello e\u0301 \U0002FA1D world");
  b2::utf8string::allocator_type a8(s8.get_allocator());

  // Push back some cps into container.
  s8.push_back(0x0301);
  hex_dump("Testing push_back of cps",s8.cbegin(),s8.cend());

  // Pop last element.
  s8.pop_back();
  hex_dump("Testing pop_back of cps",s8.cbegin(),s8.cend());

  // Push first element.
  s8.push_front(0x0301);
  hex_dump("Testing push_front of cps",s8.cbegin(),s8.cend());

  // Pop first element.
  s8.pop_front();
  hex_dump("Testing pop_front of cps",s8.cbegin(),s8.cend());

  // Print #of cps in string.
  std::cerr<<std::dec<<"s8.size(): "<<s8.size()<<", max size: "<<s8.max_size()<<std::endl;

  // Insert a cp in the middle of the string.
  b2::utf8string::const_iterator it2(s8.begin());
  ++it2;
  ++it2;
  ++it2;
  b2::utf8string::const_iterator it2_ret=s8.insert(it2,0x0301);
  hex_dump("Testing insert of cps",s8.cbegin(),s8.cend());
  std::cerr<<"insert return iterator points to: "<<std::hex<<*it2_ret<<std::endl;

  // Erase inserted codepoint.
  b2::utf8string::const_iterator it3_ret=s8.erase(it2_ret);
  hex_dump("Testing 8 erase of cp",s8.begin(),s8.end());
  std::cerr<<"erase return iterator points to: "<<std::hex<<*it3_ret<<std::endl;

  // Insert 5 equal codepoints in container.
  b2::utf8string::const_iterator it3(s8.begin());
  ++it3;++it3;++it3;++it3;
  s8.insert(it3,5,0x2FA1D);
  hex_dump("Testing 8 insert of cps",s8.begin(),s8.end());

  // Insert codepoints from a range.
  std::string insert_str(" World");
  b2::utf8string s9("Hello");;
  s9.insert(s9.end(),insert_str.begin(),insert_str.end());
  hex_dump("Testing 9 insert of range of cps",s9.begin(),s9.end());
  
  // Erase inserted codepoints.
  b2::utf8string::const_iterator it4(s8.begin()),it5;
  ++it4;++it4;++it4;++it4;
  it5=it4;
  ++it5;++it5;++it5;++it5;++it5;
  b2::utf8string::const_iterator it4_ret=s8.erase(it4,it5);
  hex_dump("Testing 8 erasing of multiple cps",s8.begin(),s8.end());
  std::cerr<<"erase return iterator points to: "<<std::hex<<*it4_ret<<std::endl;

  // Empty container.
  s8.clear();
  std::cerr<<"after clearing container, size: "<<std::dec<<s8.size()<<std::endl;

  // Test assign with two input iterators.
  b2::utf8string s10("Hello World");
  std::string str10("123456789");
  s10.assign(str10.begin(),str10.end());
  hex_dump("Testing 10 assign using iterators",s10.begin(),s10.end());

  // Test insert in an empty string.
  b2::utf8string s12;
  s12.insert(s12.end(),0x0310);
  //hex_dump("Testing 12 assign using iterators",s12.begin(),s12.end());

  // Test assign with n codepoints of same value.
  b2::utf8string s11("Hello World");
  b2::cp_t cp(0x0301);
  s11.assign(5,cp);
  hex_dump("Testing 11 assign using same codepoint 5 times",s11.begin(),s11.end());

  // NOTE! Test to see how to handle return value (reference or value) form input iterator.
  //       I believe we'll always need to return a proxy both for pre/post increment
  std::cout<<"Testing post inc"<<std::endl;
  const char*str11="Hello";
  std::list<char>str1(str11,str11+5);
  b2::const_unicode_iterator<b2::utf8_tag,std::list<char>::iterator >uit1_begin(b2::make_const_unicode_iterator<b2::utf8_tag>(str1.begin()));
  b2::cp_t const&cp1=*uit1_begin;
  uit1_begin++;
  b2::cp_t const&cp2=*uit1_begin;
  std::cerr<<std::hex<<"CHAR1: 0x"<<cp1<<std::endl;
  std::cerr<<std::hex<<"CHAR2: 0x"<<cp2<<std::endl;
  
}

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include"unicode.hpp"
#include"utf8_detail.hpp"
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace boost::unit_test;
using namespace std;
using namespace b2;

// -----------
// Utf8 tests.
// -----------
BOOST_AUTO_TEST_SUITE(utf8_tests)
  // Traits.

  // Check number of code units it takes to encode a code point.
  BOOST_AUTO_TEST_CASE(utf8_cp_encode_length){
    typedef pair<pair<cp_t,cp_t>,size_t>range_len_t;
    vector<range_len_t>range_lengths;

    // Check calculation of length to encode a code point into utf8 is correct.
    // (setup test data - code point range together with encode length, range exclusive endpoint)
    // (notice that the cp_encode_length() function does not check for validity of code point)
    range_lengths.push_back(make_pair(make_pair(0,0x80),1));
    range_lengths.push_back(make_pair(make_pair(0x80,0x800),2));
    range_lengths.push_back(make_pair(make_pair(0x800,0x10000),3));
    range_lengths.push_back(make_pair(make_pair(0x10000,detail::UNI_MAX_CP+1),4));
    for(auto const&p:range_lengths){
      cp_t start=p.first.first;
      cp_t end=p.first.second;
      size_t len=p.second;
      for(cp_t cp=start;cp<end;++cp)
        BOOST_CHECK((unicode_function_traits<utf8_tag>::cp_encode_length(cp)==len));
    }
  }
  // Check number of code units a leading byte represents.
  BOOST_AUTO_TEST_CASE(utf8_encode_length){
    typedef unicode_type_traits<utf8_tag>::cu_t cu_t;
    typedef pair<pair<cu_t,cu_t>,size_t>range_len_t;
    vector<range_len_t>cu_lengths;

    // Check correctness encoding length.
    cu_lengths.push_back(make_pair(make_pair(0x00,0x7F),1));
    cu_lengths.push_back(make_pair(make_pair(0xC3,0xDF),2));
    cu_lengths.push_back(make_pair(make_pair(0xE0,0xEF),3));
    cu_lengths.push_back(make_pair(make_pair(0xF0,0xF4),4));
    for(auto const&p:cu_lengths){
      cu_t start=p.first.first;
      cu_t end=p.first.second;
      cu_t len=p.second;
      for(cu_t cu=start;cu<=end;++cu){
        size_t cu_len=0;
        BOOST_CHECK((unicode_function_traits<utf8_tag>::cu_encode_length(cu,cu_len)==uni_error::no_error));
        BOOST_CHECK(cu_len==len);
      }
    }
    // Check that we detect invalid lead bytes.
    for(size_t cu=0;cu<0x100;++cu){
      if((cu>=0xC2&&cu<=0xF5)||(cu<=0x80))continue;
      size_t cu_len;
      BOOST_CHECK((unicode_function_traits<utf8_tag>::cu_encode_length(cu,cu_len)==uni_error::error_invalid_lead_byte));
    }
  }
  BOOST_AUTO_TEST_CASE(utf8_trailing_byte){
    typedef unicode_type_traits<utf8_tag>::cu_t cu_t;

    // Check that we correctly identify trailing bytes as trailing bytes.
    for(cu_t cu=0x80;cu<=0xBF;++cu){
      BOOST_CHECK((b2::detail::utf8_cu_is_trail_byte(cu)));
    }
    // Check that we detect invalid trailing bytes.
    for(size_t cu=0;cu<=0xFF;++cu){
      if(cu>=0x80&&cu<=0xBF)continue;
      BOOST_CHECK((!b2::detail::utf8_cu_is_trail_byte(static_cast<cu_t>(cu))));
    }
  }
  // Check that decoding from utf8 to code points works.
  BOOST_AUTO_TEST_CASE(utf8_decode){
    // NOTE! Should check more encodings - this is only a sample test.
    typedef unicode_type_traits<utf8_tag>::cu_t cu_t;
    typedef cu_t*iter_t;
    cu_t cus_[]={0xE2,0x82,0xAC};
    cu_t*cus=&cus_[0];
    cp_t cp;
    iter_t end=&cus_[0];
    advance(end,3);
    iterator_traits<iter_t>::difference_type cu_len;
    uni_error::error_code err=unicode_function_traits<utf8_tag>::decode(cus,cp,&end,cu_len);
    BOOST_CHECK(err==uni_error::no_error);
    BOOST_CHECK(cu_len==3);
    BOOST_CHECK(cp=0x20ac);
  }
  BOOST_AUTO_TEST_CASE(utf8_encode){
    // NOTE! Should do more code points - this is only a sample test.
    typedef unicode_type_traits<utf8_tag>::cu_t cu_t;
    cu_t cus[4];
    cu_t*cus_ptr=&cus[0];
    cp_t cp=0x20ac;
    uni_error::error_code err=unicode_function_traits<utf8_tag>::encode(cp,cus_ptr);
    BOOST_CHECK(err==uni_error::no_error);
    BOOST_CHECK(cus_ptr==&cus[0]+3);
    BOOST_CHECK(cus[0]==0xE2&&cus[1]==0x82&&cus[2]==0xAC);
  }
BOOST_AUTO_TEST_SUITE_END()


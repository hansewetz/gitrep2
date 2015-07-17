// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#ifndef __TRAITS_HPP__
#define __TRAITS_HPP__
#include"utf8_detail.hpp"
#include<type_traits>
namespace b2{

// ----------- (tags) Encoding markers.
struct utf8_tag{};

// ----------- (traits) base unicode_traits structure.
template<typename EncodeTag>struct unicode_traits;

// NOTE! Should not delegate in these functions - instead, just implement the function here.
//	 move these two structures into utf8_detail.hpp and just implement them inline

// ----------- (traits) utf8 traits.
template<>struct unicode_traits<utf8_tag>{
  // Size in bytes of a code unit.
  using cu_t=detail::utf8_cu_t;

  // Get maximum number of code units when encoding a codepoint.
  constexpr static std::size_t max_cu_encode_length=4;

  // Get number of code units to encode a code point.
  static std::size_t cp_encode_length(cp_t cp){return detail::utf8_cp_encode_length(cp);}

  // Get #code units used to encode code point given first code unit of encoding.
  // (notice that the function returns error_invalid_lead_byte if the byte is not a start byte)
  template<typename DifferenceType>
  static uni_error::error_code cu_encode_length(cu_t u,DifferenceType&diff){
    return detail::utf8_cu_encode_length(u,diff);
  }
  // Convert a seqeuence of code units to a code point.
  template<typename InputIt>
  static uni_error::error_code decode(InputIt&it,cp_t&cp,typename std::add_pointer<InputIt>::type end,typename std::iterator_traits<InputIt>::difference_type&cu_len){
    return detail::utf8_decode(it,cp,end,cu_len);
  }
  // Convert a code point to a sequence of code units.
  template<typename OutputIt>
  static uni_error::error_code encode(cp_t cp,OutputIt&it){
    return detail::utf8_encode(cp,it);
  }
};
}
#endif

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#ifndef __UTF8_DETAIL_HPP__
#define __UTF8_DETAIL_HPP__
#include "detail.hpp"
#include "types.hpp"
#include "error.hpp"
#include<iterator>
#include<boost/type_traits.hpp>
namespace b2{
namespace detail{

// NOTE! Possibly get rid of b2:: everywhere.

// ----------- UTF8 stuff.
typedef b2::uint8_t utf8_cu_t;

// Check if byte is a trailing byte.
static bool utf8_cu_is_trail_byte(utf8_cu_t u){
  // check for valid trail together with invalid bytes C0, C1, F5-FF.
  // C0 and C1 can only be used for overlong encoding of basic ASCII characters.
  // F4-F5 can only encode cps tha are greater than max cp max value.
// NOTE! #define U8_IS_TRAIL(c) (((c)&0xc0)==0x80)
// NOTE! #define U8_IS_LEAD(c) ((uint8_t)((c)-0xc0)<0x3e)
  return ((u>>6)==0x02)&&!(u==0xC0||u==0xC1||(u>=0xF5&&u<=0xFF));
}
// Get length of encoded codepoint.
// (assumes cp is valid)
static size_t utf8_cp_encode_length(cp_t cp){
  if(cp<0x80)return 1;
  if(cp<0x0800)return 2;
  if(cp<0x010000)return 3;
  return 4;
}
// Check if cp is overlong.
template<typename Difference_type>
static bool utf8_cp_is_overlong(b2::cp_t cp,Difference_type len){
  if(utf8_cp_encode_length(cp)==static_cast<size_t>(len))return false;
  return true;
}
// Get length of code units by checking leading byte.
template<typename DifferenceType>
static b2::uni_error::error_code utf8_cu_encode_length(utf8_cu_t u,DifferenceType&diff){
  // Notice we must view 0xC0,0xC1,0xF4-0xFF as invalid bytes.
  if(u<0x80)diff=static_cast<DifferenceType>(1);else
  if((u>>5)==0x06){
    if(u!=0xC0&&u!=0xC1)diff=static_cast<DifferenceType>(2);
    else return b2::uni_error::error_invalid_lead_byte;
  }else
  if((u>>4)==0x0E)diff=static_cast<DifferenceType>(3);else
  if(((u>>3)==0x1E)&&(u<0xF5))diff=static_cast<DifferenceType>(4);
  else return b2::uni_error::error_invalid_lead_byte;
  return b2::uni_error::no_error;
}
// Helper function for checking range error.
template<typename InputIt>
inline static bool check_range_error(InputIt&it,InputIt&end){
  if(&end&&end==it)return false;
  return true;
}
// Convert code unit to code point.
template<typename InputIt>
static b2::uni_error::error_code utf8_decode(InputIt&it,b2::cp_t&cp,typename boost::add_pointer<InputIt>::type end,typename std::iterator_traits<InputIt>::difference_type*cu_len){
  typedef typename std::iterator_traits<InputIt>::difference_type difference_type;
  difference_type tmp_len;
  b2::uni_error::error_code err=utf8_cu_encode_length(*it,tmp_len);
  if(err)return err;
  cp=static_cast<b2::cp_t>(*it);
  switch(tmp_len){
    case 1:
      break;
    case 2:
      ++it;
      if(!check_range_error(it,*end))return b2::uni_error::error_range;
      if(!utf8_cu_is_trail_byte(*it))return b2::uni_error::error_invalid_trail_byte;
      cp=((cp<<6)&0x07FF)+((*it)&0x3F);
      break;
    case 3:
      ++it;
      if(!check_range_error(it,*end))return b2::uni_error::error_range;
      if(!utf8_cu_is_trail_byte(*it))return b2::uni_error::error_invalid_trail_byte;
      cp=((cp<<12)&0xFFFF)+((static_cast<b2::cp_t>(*it)<<6)&0xFFF);
      ++it;
      if(!check_range_error(it,*end))return b2::uni_error::error_range;
      if(!utf8_cu_is_trail_byte(*it))return b2::uni_error::error_invalid_trail_byte;
      cp+=(*it)&0x3F;
      break;
    case 4:
      ++it;
      if(!check_range_error(it,*end))return b2::uni_error::error_range;
      if(!utf8_cu_is_trail_byte(*it))return b2::uni_error::error_invalid_trail_byte;
      cp=((cp<<18)&0x1FFFFF)+((static_cast<b2::cp_t>(*it)<<12)&0x3FFFF);
      ++it;
      if(!check_range_error(it,*end))return b2::uni_error::error_range;
      if(!utf8_cu_is_trail_byte(*it))return b2::uni_error::error_invalid_trail_byte;
      cp+=((static_cast<b2::cp_t>(*it))<<6)&0xFFF;
      ++it;
      if(!check_range_error(it,*end))return b2::uni_error::error_range;
      if(!utf8_cu_is_trail_byte(*it))return b2::uni_error::error_invalid_trail_byte;
      cp+=static_cast<utf8_cu_t>(*it)&0x3F;
      break;
    default:
      return b2::uni_error::error_invalid_lead_byte;
  }
  // check that cp is valid and that is not encoded with too many bytes,
  if(uni_is_valid_cp(cp)==false)return b2::uni_error::error_invalid_cp;
  if(utf8_cp_is_overlong(cp,tmp_len))return b2::uni_error::error_overlong_cp;
  if(cu_len)*cu_len=tmp_len;
  return b2::uni_error::no_error;
}

// Convert code point to code units.
template<typename OutputIt>
static b2::uni_error::error_code utf8_encode(cp_t cp,OutputIt&it){
  if(!uni_is_valid_cp(cp))return b2::uni_error::error_invalid_cp;;
  if(cp<0x80){
    *it++=static_cast<utf8_cu_t>(cp);
  }else
  if(cp<0x800){
    *it++=static_cast<utf8_cu_t>((cp>>6)|0xC0);
    *it++=static_cast<utf8_cu_t>((cp&0x3F)|0x80);
  }else
  if(cp<0x10000){
    *it++=static_cast<utf8_cu_t>((cp>>12)|0xE0);
    *it++=static_cast<utf8_cu_t>(((cp>>6)&0x3F)|0x80);
    *it++=static_cast<utf8_cu_t>((cp&0x3F)|0x80);
  }
  else{
    *it++=static_cast<utf8_cu_t>((cp>>18)|0xF0);
    *it++=static_cast<utf8_cu_t>(((cp>>12)&0x3F)|0x80);
    *it++=static_cast<utf8_cu_t>(((cp>>6)&0x3F)|0x80);
    *it++=static_cast<utf8_cu_t>((cp&0x3F)|0x80);
  }
  return b2::uni_error::no_error;
}
}
}
#endif

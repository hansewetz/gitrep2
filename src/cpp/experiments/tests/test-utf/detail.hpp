// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#ifndef __DETAIL_H__
#define __DETAIL_H__
#include "types.hpp"
namespace b2{
namespace detail{

// ----------- Dummy iterator when needed to supply one.
typedef char*dummy_iterator;

// ----------- Internal constants.
const b2::uint32_t UNI_MAX_CP=0x0010FFFFu;
const b2::uint32_t UNI_UTF16_LEAD_SURROGATE_MIN=0x0000D800u;
const b2::uint32_t UNI_UTF16_TRAIL_SURROGATE_MAX=0x0000DFFFu;

// ----------- Internal general utilities.
// Check if a code point is valid.
static bool uni_is_valid_cp(b2::cp_t cp){
    return // Must be less than max cp.
           cp<=UNI_MAX_CP
           &&
           // No surrogate cps.
           !(cp>=UNI_UTF16_LEAD_SURROGATE_MIN&&cp<=UNI_UTF16_TRAIL_SURROGATE_MAX)
           &&
           // Last two cps on each of the 17 planes are excluded.
           !((cp&0xFFFEu)==0xFFFEu)
           &&
           // Unicode 3.2, cps 0xFDD0 ... 0xFDEF are not valid cps.
           !(cp>=0xFDD0u&&cp<=0xFDEFu);
}
}
}
#endif

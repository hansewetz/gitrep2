#ifndef __SEG_BASE_H__
#define __SEG_BASE_H__
#include <iosfwd>
namespace xlate{

// base class for segmenters
class SegBase{
public:
  // ctor, assifgn, dtor
  SegBase()=default;
  SegBase(SegBase const&)=default;
  SegBase(SegBase&&)=default;
  SegBase&operator=(SegBase const&)=default;
  SegBase&operator=(SegBase&&)=default;
  virtual~SegBase()=default;

  // debug print
  virtual std::ostream&print(std::ostream&)const=0;
};
}
#endif

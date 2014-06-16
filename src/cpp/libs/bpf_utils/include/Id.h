#ifndef __ID_H__
#define __ID_H__
#include "bpf_utils/utility.h"
#include <iosfwd>
#include <string>
#include <utility>
#include "boost/lexical_cast.hpp"

// serialization stuff
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/string.hpp>
namespace bpf{

// identifier of an entity
template<typename E,typename V=std::string,bool UUID=false>
class Id{
public:
  // ctors,assign,dtor (copyable and movable)
  Id(V const&val):val_(val){}
  Id(Id const&)=default;
  Id(Id&&)=default;
  Id&operator=(Id const&)=default;
  Id&operator=(Id&&)=default;
  ~Id()=default;

  // get identifier in string format
  std::string asString()const{return boost::lexical_cast<std::string>(val_);}

  // comparison operators
  bool operator==(Id const&id)const{return val_==id.val_;}
  bool operator<(Id const&id)const{return val_<id.val_;}
private:
  // id generated internally
  V val_;

  // serialization stuff
  Id()=default;
  friend class boost::serialization::access;
  template<typename Archive>friend void serialize(Archive&ar,Id&id,const unsigned int version){
    using boost::serialization::make_nvp;
    ar&make_nvp("id",id.val_);
  }
};
// specialization for UUID generated identifiers
template<typename E,typename V>
class Id<E,V,true>{
public:
  // ctors,assign,dtor (copyable and movable)
  Id():val_(generateUuid()){}
  Id(Id const&)=default;
  Id(Id&&)=default;
  Id&operator=(Id const&)=default;
  Id&operator=(Id&&)=default;
  ~Id()=default;

  // get identifier in string format
  std::string asString()const{return boost::lexical_cast<std::string>(val_);}

  // comparison operators
  bool operator==(Id const&id)const{return val_==id.val_;}
  bool operator<(Id const&id)const{return val_<id.val_;}
private:
  // id generated internally
  V val_;

  // serialization stuff
  friend class boost::serialization::access;
  template<typename Archive>friend void serialize(Archive&ar,Id&id,const unsigned int version){
    using boost::serialization::make_nvp;
    ar&make_nvp("id",id.val_);
  }
};
// print function
template<typename E,typename V,bool UUID>
std::ostream&operator<<(std::ostream&os,Id<E,V,UUID>const&id){
  return os<<id.asString();
}
// make sure we get all relational operators
using namespace std::rel_ops;
}
#endif

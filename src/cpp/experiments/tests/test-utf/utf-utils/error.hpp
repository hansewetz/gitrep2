// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#ifndef __ERROR_HPP__
#define __ERROR_HPP__
#include<iostream>
#include<string>
#include<stdexcept>
namespace b2{

// ----------- (error) Error codes.
class uni_error{
public:
enum error_code{no_error=0,error_invalid_lead_byte=1,error_invalid_trail_byte=2,error_overlong_cp=3,error_invalid_cp=4,error_range=5};
  // ctors etc.
  uni_error():err_(no_error){}
  explicit uni_error(error_code err):err_(err){}
  uni_error(uni_error const&uerr)=default;
  uni_error(uni_error&&uerr)=default;
  uni_error&operator=(uni_error const&uerr)=default;
  uni_error&operator=(uni_error&&uerr)=default;
  ~uni_error()=default;

  // getters
  error_code get_error_code()const{return err_;}
  std::string const&get_error_msg()const{return err_msgs_[err_];}

  // conversion to bool
  operator bool()const{return err_!=no_error;}

  // debug print method
  std::ostream&print(std::ostream&os)const{return os<<err_msgs_[err_];}
private:
  error_code err_;
  static std::string const err_msgs_[];
};
std::string const uni_error::err_msgs_[]={"no error","invalid lead byte","invalid trail byte","overlong code point","invalid code point","range error"};
std::ostream&operator<<(std::ostream&os,uni_error const&uerr){return uerr.print(os);}

// ----------- (exception) Logic exception.
class uni_exception:public std::logic_error{
public:
  // ctors etc.
  explicit uni_exception(uni_error const&uerr):logic_error(uerr.get_error_msg()),uerr_(uerr){}
  uni_exception(uni_exception const&)=default;
  uni_exception(uni_exception&&)=default;
  uni_exception&operator=(uni_exception const&)=default;
  uni_exception&operator=(uni_exception&&)=default;
  ~uni_exception()=default;

  // getters
  uni_error const&error()const{return uerr_;}
private:
  uni_error uerr_;
};
}
#endif

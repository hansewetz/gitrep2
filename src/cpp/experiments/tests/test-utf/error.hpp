#ifndef __ERROR_HPP__
#define __ERROR_HPP__
#include<iostream>
#include<stdexcept>
namespace b2{

// ----------- (error) Error codes.
class uni_error{
public:
enum error_code{no_error=0,error_invalid_lead_byte=1,error_invalid_trail_byte=2,error_overlong_cp=3,error_invalid_cp=4,error_range=5};
  uni_error():err_(no_error){}
  explicit uni_error(error_code err):err_(err){}
  explicit uni_error(uni_error const&uerr):err_(uerr.err_){}
  uni_error&operator=(uni_error const&uerr){err_=uerr.err_;return*this;}
  error_code get_error_code()const{return err_;}
  operator bool()const{return err_!=no_error;}
  std::string const&get_error_msg()const{return err_msgs_[err_];}
  std::ostream&print(std::ostream&os)const{return os<<err_msgs_[err_];}
private:
  error_code err_;
  static std::string const err_msgs_[];
};
std::string const uni_error::err_msgs_[]={"no error","invalid lead byte","invalid trail byte","overlong code point","invalid code point","range error"};
std::ostream&operator<<(std::ostream&os,uni_error const&uerr){return uerr.print(os);}

// ----------- (execption) Logic exception.
class uni_exception:public std::logic_error{
public:
  explicit uni_exception(uni_error const&uerr):logic_error(uerr.get_error_msg()),uerr_(uerr){}
  uni_error const&error()const{return uerr_;}
private:
  uni_error uerr_;
};
}
#endif

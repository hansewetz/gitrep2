#ifndef __TUPLE_UTILS_H__
#define __TUPLE_UTILS_H__
#include <ostream>
#include <tuple>
#include <utility>
#include <cxxabi.h>
#include <type_traits>
namespace mtdata{
//-----------------------------------------
// meta function to check if a type is a tuple
template<class>struct is_tuple:std::false_type{}; 
template<class... T> struct is_tuple<std::tuple<T...>>:std::true_type {};

//-----------------------------------------
// print a variable together with its type 
template<typename T>
void print_type_value(std::ostream&os,T const&t){
  int status;
  os<<"("<<abi::__cxa_demangle(typeid(T).name(),0,0,&status)<<": "<<t<<")";
}
// print a tuplet (type and value)
template<std::size_t N,typename Tuple>
struct print_tuple{
  static void print(std::ostream&os,Tuple const&t){
    print_tuple<N-1,decltype(t)>::print(os,t);
    print_type_value(os,std::get<N-1>(t));
  }
};
template<typename Tuple>
struct print_tuple<0,Tuple>{
  static void print(std::ostream&os,Tuple const&t){}
};
// tuplet print operator (no need to have perfect forwarding since tuple can always be const)
template<typename ... Args>
std::ostream&operator<<(std::ostream&os,std::tuple<Args ...>const&t){
  os<<"[";
  print_tuple<sizeof ... (Args),decltype(t)>::print(os,t);
  return os<<"]";
}
//-----------------------------------------
// apply function with signature void(*)(int,T t) to each tuple element
template<std::size_t N,typename Func,typename Tuple>
struct apply_with_index_t{
  static void apply(Func f,Tuple&&t){
    apply_with_index_t<N-1,Func,Tuple>::apply(f,std::forward<Tuple>(t));
    f(N-1,std::forward<decltype(std::get<N-1>(t))>(std::get<N-1>(t)));
  }
};
template<typename Func,typename Tuple>
struct apply_with_index_t<0,Func,Tuple>{
  static void apply(Func f,Tuple&&t){}
};
// apply a function object to each element in a tuple, passing index and tuplet element as parameters
// (constrain function to handle only tuplets)
// (implements perfect forwarding)
template<typename Func,typename Tuple> 
typename std::enable_if<is_tuple<typename std::decay<Tuple>::type>::value>::type 
apply_with_index(Func f,Tuple&&t){
  typedef typename std::decay<Tuple>::type TupleNonRef;
  static const std::size_t N=std::tuple_size<TupleNonRef>::value;
  apply_with_index_t<N,Func,Tuple>::apply(f,std::forward<Tuple>(t));
}
//-----------------------------------------
// apply function with signature void apply<int>(T t) to each tuple element
template<std::size_t N,typename Func,typename Tuple>
struct apply_with_index_template_t{
  static void apply(Func f,Tuple&&t){
    apply_with_index_template_t<N-1,Func,Tuple>::apply(f,std::forward<Tuple>(t));
    f.template apply<N-1>(std::forward<decltype(std::get<N-1>(t))>(std::get<N-1>(t)));
  }
};
template<typename Func,typename Tuple>
struct apply_with_index_template_t<0,Func,Tuple>{
  static void apply(Func f,Tuple&&t){}
};
// apply a function object to each element in a tuple, passing index as template parameter and tuplet element as parameter
// (constrain function to handle only tuplets)
// (implements perfect forwarding)
template<typename Func,typename Tuple> 
typename std::enable_if<is_tuple<typename std::decay<Tuple>::type>::value>::type 
apply_with_index_template(Func f,Tuple&&t){
  typedef typename std::decay<Tuple>::type TupleNonRef;
  static const std::size_t N=std::tuple_size<TupleNonRef>::value;
  apply_with_index_template_t<N,Func,Tuple>::apply(f,std::forward<Tuple>(t));
}
//-----------------------------------------

// make a tuple type having all N elements of same type
template<std::size_t N,typename T>
struct uniform_tuple_builder{
  using type=decltype(std::tuple_cat(std::declval<std::tuple<T>>(),std::declval<typename uniform_tuple_builder<N-1,T>::type>()));
};
template<typename T>
struct uniform_tuple_builder<1,T>{
  using type=std::tuple<T>;
};
}
#endif

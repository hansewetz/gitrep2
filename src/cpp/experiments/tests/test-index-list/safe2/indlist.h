#ifndef __TUTILS_H__
#define __TUTILS_H__
#include <iostream>
#include <tuple>
#include <type_traits>
namespace tutils{

// -----------------------------------------------------------------------------
// indlist
// -----------------------------------------------------------------------------

// forward decl.
template<typename IL>struct reverse;
template<std::size_t a,typename IL>struct push_back;
template<std::size_t a,typename IL>struct push_front;
template<std::size_t N,typename IL>struct popn_front;
template<typename Tuple>struct make_indlist_from_tuple;
template<typename IL,typename F,typename T>struct apply_tuple_with_indlist_ret;
struct maketuple;
template<template<class>class FIT,
         typename T,
         typename TD=typename std::decay<T>::type,
         typename IL=typename make_indlist_from_tuple<TD>::type,
         typename IR=typename FIT<IL>::type,
         typename F=maketuple,
         typename R=typename apply_tuple_with_indlist_ret<IR,F,T>::type>
constexpr R transform_tuple(T&&t);
template<std::size_t N,typename T,
         typename F=maketuple,
         typename TD=typename std::decay<T>::type,
         typename IL=typename popn_front<N,typename make_indlist_from_tuple<TD>::type>::type,
         typename R=typename apply_tuple_with_indlist_ret<IL,F,T>::type>
constexpr auto popn_front_tuple(T&&t)->R;

// indlist
template<std::size_t...i>struct indlist{};

// make an indlist from [start ... end]
template<std::size_t start,std::size_t end>
struct make_indlist_from_range{
  using type=typename push_front<start,typename make_indlist_from_range<start+1,end>::type>::type;
};
template<std::size_t i>
struct make_indlist_from_range<i,i>{
  using type=indlist<i>;
};
// make an indlist with N elements of value V
template<std::size_t N,std::size_t V>
struct make_uniform_indlist{
  using type=typename push_back<V,typename make_uniform_indlist<N-1,V>::type>::type;
};
template<std::size_t V>
struct make_uniform_indlist<0,V>{
  using type=indlist<>;
};
// make an indlist from a tuple
template<typename Tuple>struct make_indlist_from_tuple;
template<typename...Args>
struct make_indlist_from_tuple<std::tuple<Args...>>{
  using type=typename make_indlist_from_range<0,sizeof...(Args)-1>::type;
};
template<>
struct make_indlist_from_tuple<std::tuple<>>{
  using type=indlist<>;
};
// get size of an indlist
template<typename IL>struct size;
template<std::size_t...i>
struct size<indlist<i...>>{
  static std::size_t const value=sizeof...(i);
};
// push front an index to an indlist
template<std::size_t a,typename IL>struct push_front;
template<std::size_t a,size_t...i>
struct push_front<a,indlist<i...>>{
  using type=indlist<a,i...>;
};
// push back an index to an indlist
template<std::size_t a,typename IL>struct push_back;
template<std::size_t a,size_t...i>
struct push_back<a,indlist<i...>>{
  using type=indlist<i...,a>;
};
// pop front index from indlist
template<typename IL>struct pop_front;
template<std::size_t i,std::size_t...j>
struct pop_front<indlist<i,j...>>{
  using type=indlist<j...>;
};
// pop back element from indlist
// (reverse, pop front, reverse)
template<typename IL>
struct pop_back{
  using R0=typename reverse<IL>::type;
  using R1=typename pop_front<R0>::type;
  using type=typename reverse<R1>::type;
};
// pop n elements from front
template<std::size_t N,typename IL>
struct popn_front{
  using TMP=typename pop_front<IL>::type;
  using type=typename popn_front<N-1,TMP>::type;
};
template<typename IL>
struct popn_front<0,IL>{
  using type=IL;
};
// pop n elements from back
template<std::size_t N,typename IL>
struct popn_back{
  using TMP=typename pop_back<IL>::type;
  using type=typename popn_back<N-1,TMP>::type;
};
template<typename IL>
struct popn_back<0,IL>{
  using type=IL;
};
// get first element as an indlist
template<typename IL>struct front;
template<std::size_t i,std::size_t...j>
struct front<indlist<i,j...>>{
  using type=indlist<i>;
};
// get last element as an indlist
template<typename IL>
struct back{
  using R=typename reverse<IL>::type;
  using type=typename front<R>::type;
};
// get first N elements as an indlist
template<std::size_t N,typename IL>
struct firstn{
  using type=typename popn_back<size<IL>::value-N,IL>::type;
};
// get last N elements as an indlist
template<std::size_t N,typename IL>
struct lastn{
  using R1=typename reverse<IL>::type;
  using R2=typename firstn<N,R1>::type;
  using type=typename reverse<R2>::type;
};
// get an element as a size_t gven the index
template<std::size_t IND,typename IL>struct get;
template<std::size_t IND,std::size_t i,std::size_t...j>
struct get<IND,indlist<i,j...>>{
  static constexpr std::size_t value=get<IND-1,indlist<j...>>::value;
};
template<std::size_t i,std::size_t...j>
struct get<0,indlist<i,j...>>{
  static constexpr std::size_t value=i;
};
// append an indlist to another indlist
template<typename IL1,typename IL2>struct append;
template<typename IL1,std::size_t i,std::size_t...j>
struct append<IL1,indlist<i,j...>>{
  using type=typename append<typename push_back<i,IL1>::type,indlist<j...>>::type;
};
template<typename IL1>
struct append<IL1,indlist<>>{
  using type=IL1;
};
// reverse an indlist (re-use push_back)
template<std::size_t i,size_t...j>
struct reverse<indlist<i,j...>>{
  using R=typename reverse<indlist<j...>>::type;
  using type=typename push_back<i,R>::type;
};
template<>
struct reverse<indlist<>>{
  using type=indlist<>;
};
// get a slice of an indlist
template<std::size_t IND,std::size_t LEN,typename IL>
struct slice{
  using IL1=typename popn_front<IND,IL>::type;
  using type=typename popn_back<size<IL1>::value-LEN,IL1>::type;
};
// reverse elements starting from I and N elements forward
template<std::size_t IND,std::size_t N,typename IL>
struct reverse_slice{
  using IL_LEFT=typename slice<0,IND,IL>::type;
  using IL_MID=typename slice<IND,N,IL>::type;
  using IL_RIGHT=typename slice<IND+N,size<IL>::value-(IND+N),IL>::type;
  using IL_MID_R=typename reverse<IL_MID>::type;
  using A=typename append<IL_LEFT,IL_MID_R>::type;
  using type=typename append<A,IL_RIGHT>::type;
};
// rotate an indlist left N steps
template<std::size_t N,typename IL>
struct rotate_left{
  using R1=typename slice<0,N,typename reverse_slice<0,N,IL>::type>::type;
  using R2=typename slice<N,size<IL>::value-N,typename reverse_slice<N,size<IL>::value-N,IL>::type>::type;
  using A=typename append<R1,R2>::type;
  using type=typename reverse<A>::type;
};
// add +1 to each index in indlist
template<typename IL>struct add1;
template<std::size_t i,std::size_t...j>
struct add1<indlist<i,j...>>{
  using type=typename push_front<i+1,typename add1<indlist<j...>>::type>::type;
};
template<>
struct add1<indlist<>>{
  using type=indlist<>;
};
// fold list to an int with a binary meta function (taking two ints)
// from Wikipedia: 
//                  foldl1 f [x]      = x
//                  foldl1 f (x:y:xs) = foldl1 f (f x y : xs)
template<template<std::size_t,std::size_t>class FBIN,typename IL>struct foldl_indlist2int;
template<template<std::size_t,std::size_t>class FBIN,std::size_t i,std::size_t j>
struct foldl_indlist2int<FBIN,indlist<i,j>>{
  constexpr static std::size_t value=FBIN<i,j>::value;
};
template<template<std::size_t,std::size_t>class FBIN,std::size_t i,std::size_t j,std::size_t...k>
struct foldl_indlist2int<FBIN,indlist<i,j,k...>>{
  constexpr static std::size_t value=foldl_indlist2int<FBIN,indlist<FBIN<i,j>::value,k...>>::value;
};

// -----------------------------------------------------------------------------
// printing
// -----------------------------------------------------------------------------

// print an indlist object
template<std::size_t i,std::size_t...j>
std::ostream&operator<<(std::ostream&os,indlist<i,j...>const&){
  return os<<i<<" "<<indlist<j...>();
}
std::ostream&operator<<(std::ostream&os,indlist<>const&){
}
// print a tuple object
template<std::size_t N,typename T>
struct tuple_printer{
  static constexpr void print(std::ostream&os,T const&t){
    tuple_printer<N-1,T>::print(os,t);
    os<<std::get<N-1>(t)<<" ";
  }
};
template<typename T>
struct tuple_printer<0,T>{
  static constexpr void print(std::ostream&os,T const&t){}
};
template<typename ... Args>
std::ostream&operator<<(std::ostream&os,std::tuple<Args ...>const&t){
  os<<"[";
  tuple_printer<sizeof ... (Args),decltype(t)>::print(os,t);
  return os<<"]";
}
// -----------------------------------------------------------------------------
// calling functions with the help of indlist
// -----------------------------------------------------------------------------

// dummy meta function returning 'return type' for 'apply_tuple_with_indlist'
// (must use this since expressing teh return type directly won't work since the return type depends on the entire function having been declared)
// ('apply_tuple','apply_tuple_with_indlist','apply_with_indlist'  use this function to retrieve the return type)
template<typename IL,typename F,typename T>struct apply_tuple_with_indlist_ret;
template<std::size_t...i,typename F,typename T>
struct apply_tuple_with_indlist_ret<indlist<i...>,F,T>{
  using type=typename std::result_of<F(decltype(std::get<i>(std::forward<T>(std::declval<T>())))...)>::type;
};
// pass tuple to a function taking list of arguments (controll which args and the order using indlist)
template<std::size_t...i,typename F,typename T,typename R=typename apply_tuple_with_indlist_ret<indlist<i...>,F,T>::type>
constexpr auto apply_tuple_with_indlist(indlist<i...>const&,F f,T&&tu)->R
{
  return f(std::get<i>(std::forward<T>(tu))...);
}
// dummy meta function returning 'return type' for 'fapply_tuple_with_indlist'
template<typename IL,typename F,typename FX,typename T>struct fapply_tuple_with_indlist_ret;
template<std::size_t...i,typename F,typename FX,typename T>
struct fapply_tuple_with_indlist_ret<indlist<i...>,F,FX,T>{
  using type=decltype(std::declval<F>()(std::declval<FX>()(std::get<i>(std::forward<T>(std::declval<T>())))...));
};
// pass tuple to a function taking list of FX(arguments) (controll which args and the order using indlist)
template<std::size_t...i,typename F,typename FX,typename T,typename R=typename fapply_tuple_with_indlist_ret<indlist<i...>,F,FX,T>::type>
constexpr auto fapply_tuple_with_indlist(indlist<i...>const&,F f,FX fx,T&&tu)->R
{
  return f(fx(std::get<i>(std::forward<T>(tu)))...);
}
// pass tuple to a function taking list of arguments
template<typename F,typename T,
         typename TD=typename std::decay<T>::type,
         typename IL=typename make_indlist_from_tuple<TD>::type,
         typename R=typename apply_tuple_with_indlist_ret<IL,F,T>::type>
constexpr auto apply_tuple(F f,T&&tu)->R
{
  return apply_tuple_with_indlist(IL(),f,std::forward<T>(tu));
}
// pass list of types to a function (control order of arguments to F with an 'indlist')
template<std::size_t...i,typename F,typename...T,
         typename IL=indlist<i...>,
         typename TU=std::tuple<T...>,
         typename R=typename apply_tuple_with_indlist_ret<IL,F,TU>::type>
constexpr auto apply_with_indlist(IL,F f,T&&...t)->R
{
  // cannot have more than one statement here (won't compile: gcc4.8 ?)
  return apply_tuple_with_indlist(IL(),f,TU{std::forward<T>(t)...});
}
// pass each element of a tuple to a function (control call order using 'indlist')
template<std::size_t i,std::size_t...j,typename F,typename T>
constexpr void apply_tuple_ntimes_with_indlist(indlist<i,j...>const&,F f,T&&tu)
{
  f(std::get<i>(std::forward<T>(tu)));
  apply_tuple_ntimes_with_indlist(indlist<j...>(),f,std::forward<T>(tu));
};
template<typename F,typename T>
constexpr void apply_tuple_ntimes_with_indlist(indlist<>const&,F f,T&&tu){
}
// call a function passing one element from each of a set of tuples (control which elemnt using an indlist)
template<std::size_t...i,typename F,typename...T>
constexpr auto apply_elem_tuples(indlist<i...>const&,F f,T&&...t)->decltype(f(std::get<i>(std::forward<T>(t))...))
{
  return f(std::get<i>(std::forward<T>(t))...);
}
// apply a function on the nth element of a list of tuples
template<std::size_t N,typename F,typename...T,typename IL=typename make_uniform_indlist<sizeof...(T),N>::type>
constexpr auto apply_nth_elem_tuples(F f,T&&...t)->decltype(apply_elem_tuples(IL(),f,std::forward<T>(t)...))
{
  return apply_elem_tuples(IL(),f,std::forward<T>(t)...);
}
// fold a a tuple (binary function)
// (truly horrific syntax)
struct foldl_tuple{
  template<typename F,typename T,typename U,typename...V,
           typename TU=std::tuple<T,U,V...>,
           typename FRONT=decltype(std::declval<F>()(std::get<0>(std::declval<TU>()),std::get<1>(std::declval<TU>()))),
           typename TAIL=decltype(popn_front_tuple<2>(std::declval<TU>()))>
  auto operator()(F f,std::tuple<T,U,V...>const&tu)->decltype((*this)(f,std::tuple_cat(std::tuple<FRONT>(std::declval<FRONT>()),std::declval<TAIL>()))){
    FRONT front=f(std::get<0>(tu),std::get<1>(tu));
    TAIL tail=popn_front_tuple<2>(tu);
    return operator()(f,std::tuple_cat(std::tuple<FRONT>(front),tail));
  }
  template<typename F,typename T>
  constexpr auto operator()(F f,std::tuple<T>const&tu)->T{
    return std::get<0>(tu);
  }
};
template<typename F,typename T>
constexpr auto ffold_tuple(F f,T&&t)->decltype(foldl_tuple()(std::declval<F>(),std::forward<T>(std::declval<T>()))){
  return foldl_tuple()(f,std::forward<T>(t));
}
// -----------------------------------------------------------------------------
// functions transforming one or more tuples into one or more tuples
// -----------------------------------------------------------------------------

// create a tuple from a list of args (not same as make_tuple - this is a structure)
struct maketuple{
  template<typename...T>
  constexpr std::tuple<T...>operator()(T&&...t){
    return std::tuple<T...>(std::forward<T>(t)...);
  }
};
// transformation of a tuple to another tuple
// (template template parameter can be 'reverse', 'pop_front', ... but must transform an indlist)
template<template<class>class FIT,
         typename T,
         typename TD=typename std::decay<T>::type,
         typename IL=typename make_indlist_from_tuple<TD>::type,
         typename IR=typename FIT<IL>::type,
         typename F=maketuple,
         typename R=typename apply_tuple_with_indlist_ret<IR,F,T>::type>
constexpr R transform_tuple(T&&t){
  return apply_tuple_with_indlist(IR(),F(),std::forward<T>(t));
}
// transform a tuple using an indlist
// (use indlist<i...> to manage overload with other 'transform_tuple' functions)
template<std::size_t...i, typename T,
         typename F=maketuple,
         typename IL=indlist<i...>,
         typename R=typename apply_tuple_with_indlist_ret<IL,F,T>::type>
constexpr R transform_tuple(indlist<i...>const&,T&&t){
    return apply_tuple_with_indlist(IL(),F(),std::forward<T>(t));
}
// transform a tuple by applying a function to each element
// (may return a tuple having different elemnt types)
template<typename FX,typename T,
         typename TD=typename std::decay<T>::type,
         typename IL=typename make_indlist_from_tuple<TD>::type,
         typename R=typename fapply_tuple_with_indlist_ret<IL,maketuple,FX,T>::type>
auto ftransform_tuple(FX fx,T&&t)->R{
    return fapply_tuple_with_indlist(IL(),maketuple(),fx,std::forward<T>(t));
}
// reverse tuple (utility function)
template<typename T,
         typename TD=typename std::decay<T>::type,
         typename IL=typename make_indlist_from_tuple<TD>::type,
         typename IR=typename reverse<IL>::type,
         typename F=maketuple,
         typename R=typename apply_tuple_with_indlist_ret<IR,F,T>::type>
constexpr R reverse_tuple(T&&t){
  return apply_tuple_with_indlist(IR(),F(),std::forward<T>(t));
}
// pop front
template<std::size_t N,typename T,
         typename F=maketuple,
         typename TD=typename std::decay<T>::type,
         typename IL=typename popn_front<N,typename make_indlist_from_tuple<TD>::type>::type,
         typename R=typename apply_tuple_with_indlist_ret<IL,F,T>::type>
constexpr auto popn_front_tuple(T&&t)->R{
  return apply_tuple_with_indlist(IL(),F(),std::forward<T>(t));
}
// pick one element in order from a set of tuples creating a new tuple
// (first elemnt is picked from first tuple at index specified by first index in indlist)
template<typename IL,typename...T,
         typename R=decltype(apply_elem_tuples(IL(),maketuple(),std::forward<T>(std::declval<T>())...))>
constexpr R transform_tuples(IL&&il,T&&...t)
{
  return apply_elem_tuples(std::forward<IL>(il),maketuple(),std::forward<T>(t)...);
}
// pick elements from a set of tuples creating a new tuple given the elemnt index
template<std::size_t N,typename...T,
         typename IL=typename make_uniform_indlist<sizeof...(T),N>::type,
         typename R=decltype(apply_elem_tuples(IL(),maketuple(),std::forward<T>(std::declval<T>())...))>
constexpr R transform_tuples(T&&...t)
{
  return apply_elem_tuples(IL(),maketuple(),std::forward<T>(t)...);
}
}
#endif

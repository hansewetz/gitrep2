#ifndef __UNICODE_HPP__
#define __UNICODE_HPP__
#include"types.hpp"
#include"error.hpp"
#include"detail.hpp"
#include"utf8_detail.hpp"
#include"traits.hpp"
#include<iostream>
#include<iterator>
#include<functional>
#include<boost/iterator/iterator_facade.hpp>
#include<boost/iterator/reverse_iterator.hpp>
#include<boost/function_output_iterator.hpp>
#include<boost/type_traits.hpp>
#include<boost/mpl/assert.hpp>
#include<boost/utility/enable_if.hpp>
#include<boost/type_traits.hpp>
#include<boost/concept_check.hpp>
#include<boost/container/deque.hpp>
namespace b2{

// -----------------------------------------------
//      Forward declarations.
// -----------------------------------------------
template<typename EncodeTag,typename Container=boost::container::deque<typename b2::unicode_type_traits<EncodeTag>::cu_t> >
class unicode_container;

// -----------------------------------------------
//      Function objects/predicates.
// -----------------------------------------------

// ---------- (function object) convert cp to cu.
// (throws exception)
// Notice that instances carry state.
template<typename EncodeTag,typename OutputIt>
class unicode_encode:public std::unary_function<cp_t,OutputIt>{
  BOOST_CONCEPT_ASSERT((boost::OutputIterator<OutputIt,cp_t>));
public:
  unicode_encode(OutputIt it):it_(it){}
  OutputIt&operator()(cp_t cp){
    uni_error::error_code err=unicode_function_traits<EncodeTag,OutputIt>::encode(cp,it_);
    if(err!=uni_error::no_error)throw uni_exception(uni_error(err));
    return it_;
  }
private:
  OutputIt it_;
};
// Helper function for creating unicode_encode object.
template<typename EncodeTag,typename OutputIt>
unicode_encode<EncodeTag,OutputIt>make_unicode_encode(OutputIt it){
  BOOST_CONCEPT_ASSERT((boost::OutputIterator<OutputIt,b2::cp_t>));
  return unicode_encode<EncodeTag,OutputIt>(it);
}
// ---------- (function object) Get #cus to encode a cp.
// (throws exception)
template<typename EncodeTag>
struct unicode_cp_encode_length:std::unary_function<cp_t,size_t>{
  size_t operator()(cp_t cp)const{
    if(!detail::uni_is_valid_cp(cp))throw uni_exception(uni_error(uni_error::error_invalid_cp));
    return unicode_function_traits<EncodeTag,detail::dummy_iterator>::cp_encode_length(cp);
  }
};
// ---------- (function object) Get #cus given first cu of encoded cp.
// (throws exeception)
template<typename EncodeTag>
struct unicode_cu_encode_length:std::unary_function<typename unicode_type_traits<EncodeTag>::cu_t,size_t>{
  size_t operator()(typename unicode_type_traits<EncodeTag>::cu_t val)const{
    size_t cu_len;
    uni_error::error_code err=unicode_function_traits<EncodeTag,detail::dummy_iterator>::cu_encode_length(val,cu_len);
    if(err!=uni_error::no_error)throw uni_exception(uni_error(err));
    return cu_len;
  }
};
// ---------- (predicate) Check that code point is a valid unicode code point.
struct unicode_cp_is_valid:std::unary_function<cp_t,bool>{
  bool operator()(cp_t cp)const{
    return detail::uni_is_valid_cp(cp);
  }
};

// ------------------------------------------
//            Iterators 
// ------------------------------------------

// NOTE! Testing.
class reference_proxy{
public:
  explicit reference_proxy():cp_(0){}
  explicit reference_proxy(cp_t cp):cp_(cp){}
  explicit reference_proxy(reference_proxy const&other):cp_(other.cp_){}
  reference_proxy const&operator=(reference_proxy const&other){cp_=other.cp_;return*this;}
  operator cp_t const&()const{return cp_;}
private:
  cp_t cp_;
};

// ---------- (iterator) constant/bidirectional iterator.
// (iterator is not dependent on a unicode_container)
// (traverses encoded codepoints and returns one codepoint at a time)
// (if failing to increment the iterator stays at the bad code point and throws an exception)
// NOTE! Reference type is a cp_t const which is not correct. Probably we should return a proxy instead.
template<typename EncodeTag,typename BidirectionalIt>
class const_unicode_iterator:public boost::iterator_facade<const_unicode_iterator<EncodeTag,BidirectionalIt>,cp_t const,boost::bidirectional_traversal_tag,reference_proxy const&>{
public:
  typedef typename std::iterator_traits<BidirectionalIt>::difference_type difference_type;
  BOOST_CONCEPT_ASSERT((boost::BidirectionalIterator<BidirectionalIt>));

  // ctor.
  const_unicode_iterator():cur_(),dirty_(true){};
  explicit const_unicode_iterator(BidirectionalIt it):cur_(it),dirty_(true){}

  // get underlying iterator.
  BidirectionalIt base()const{return cur_;}
private:
  // functions called by base iterator facade.
  friend class boost::iterator_core_access;
  void increment(){
    difference_type cu_len;
    uni_error::error_code err=unicode_function_traits<EncodeTag,BidirectionalIt>::cu_encode_length(*cur_,cu_len);
    if(err!=uni_error::no_error)throw uni_exception(uni_error(err));
    std::advance(cur_,cu_len);
    dirty_=true;
  }
  void decrement(){
    difference_type cu_len;
    while(unicode_function_traits<EncodeTag,BidirectionalIt>::cu_encode_length(*--cur_,cu_len)==uni_error::error_invalid_lead_byte);
    dirty_=true;
  }
  bool equal(const_unicode_iterator<EncodeTag,BidirectionalIt>const&other)const{
    return cur_==other.cur_;
  }
  reference_proxy const&dereference()const{
    if(!dirty_)return last_cp_;
    difference_type cu_len;
    cp_t tmp_cp;
    BidirectionalIt tmp_it(cur_);
    uni_error::error_code err=unicode_function_traits<EncodeTag,BidirectionalIt>::decode(tmp_it,tmp_cp,static_cast<typename boost::add_pointer<BidirectionalIt>::type>(0),&cu_len);
    if(err!=uni_error::no_error)throw uni_exception(uni_error(err));
    dirty_=false;
    return last_cp_=reference_proxy(tmp_cp);
  }
private:
  BidirectionalIt cur_;
  mutable reference_proxy last_cp_;
  mutable bool dirty_;
};
// make a unicode const bidirectional iterator.
template<typename EncodeTag,typename BidirectionalIt>
const_unicode_iterator<EncodeTag,BidirectionalIt>make_const_unicode_iterator(BidirectionalIt it){
  return const_unicode_iterator<EncodeTag,BidirectionalIt>(it);
}
// ---------- (iterator) input iterator converting code units to code points.
// (cannot use the boost::function_input_iterator since it derefermce in constructor)
// (making it impossible to create an end iterator)
// (remarked this on boost user group)
template<typename EncodeTag,typename InputIt>
class unicode_input_iterator:public boost::iterator_facade<unicode_input_iterator<EncodeTag,InputIt>,cp_t,boost::single_pass_traversal_tag,cp_t const>{
  BOOST_CONCEPT_ASSERT((boost::InputIterator<InputIt>));
public:
  // ctor.
  unicode_input_iterator():cur_(),dirty_(true){};
  unicode_input_iterator(InputIt it):cur_(it),dirty_(true){}
private:
  // functions called by base iterator facade.
  friend class boost::iterator_core_access;
  void increment(){
    ++cur_;
    dirty_=true;
  }
  bool equal(unicode_input_iterator<EncodeTag,InputIt>const&other)const{
    return this->cur_==other.cur_;
  }
  cp_t const dereference()const{
    if(!dirty_)return last_cp_;
    cp_t tmp_cp;
    InputIt tmp_it(cur_);
    uni_error::error_code err=unicode_function_traits<EncodeTag,InputIt>::decode(tmp_it,tmp_cp,static_cast<typename boost::add_pointer<InputIt>::type>(0),static_cast<typename std::iterator_traits<InputIt>::difference_type*>(0));
    if(err!=uni_error::no_error)throw uni_exception(uni_error(err));
    dirty_=false;
    return last_cp_=tmp_cp;
  }
private:
  InputIt cur_;
  mutable cp_t last_cp_;
  mutable bool dirty_;
};
// make a unicode input iterator.
template<typename EncodeTag,typename InputIt>
unicode_input_iterator<EncodeTag,InputIt>make_unicode_input_iterator(InputIt it){
  return unicode_input_iterator<EncodeTag,InputIt>(it);
}
// ---------- (iterator) output iterator converting code points to code units.
template<typename EncodeTag,typename OutputIt>
class unicode_output_iterator:public boost::function_output_iterator<unicode_encode<EncodeTag,OutputIt> >{
  BOOST_CONCEPT_ASSERT((boost::OutputIterator<OutputIt,b2::cp_t>));
public:
  unicode_output_iterator(OutputIt it):boost::function_output_iterator<unicode_encode<EncodeTag,OutputIt> >(make_unicode_encode<EncodeTag>(it)){}
};
// make a unicode output iterator.
template<typename EncodeTag,typename OutputIt>
unicode_output_iterator<EncodeTag,OutputIt>make_unicode_output_iterator(OutputIt it){
  BOOST_CONCEPT_ASSERT((boost::OutputIterator<OutputIt,b2::cp_t>));
  return unicode_output_iterator<EncodeTag,OutputIt>(it);
}

// -----------------------------------------------
//      Algorithms.
// -----------------------------------------------

// ---------- (algorithm) Validate encoding of a range of cus.
// (returns iterator pointing to failed code unit or end iterator if no error)
template<typename EncodeTag,typename InputIt>
InputIt unicode_validate_encoding(InputIt first,InputIt last,uni_error*uerr){
  typedef typename std::iterator_traits<InputIt>::value_type value_type;
  typedef typename std::iterator_traits<InputIt>::difference_type difference_type;
  BOOST_MPL_ASSERT((boost::is_integral<value_type>));
  BOOST_MPL_ASSERT_RELATION(sizeof(value_type),==,sizeof(typename unicode_type_traits<EncodeTag>::cu_t));
  BOOST_CONCEPT_ASSERT((boost::InputIterator<InputIt>));
  
  while(first!=last){
    cp_t tmp_cp;
    InputIt tmp_it(last);
    difference_type cu_len;
    uni_error::error_code err=unicode_function_traits<EncodeTag,InputIt>::decode(tmp_it,tmp_cp,&last,&cu_len);
    if(err!=uni_error::no_error){
      if(uerr)*uerr=uni_error(err);
      return first;
    }
    std::advance(first,cu_len);
  }
  return last;
}

// -----------------------------------------------
//      Container.
// -----------------------------------------------

// ---------- (container) Container adapter for unicode sequences.
// Data with which the container is fed must be correct or the container will throw an exception.
template<typename EncodeTag,typename Container>
class unicode_container{
private:
  Container cont_;
  struct enabler{};
public:
  // standard typedefs.
  typedef typename Container::allocator_type allocator_type;
  typedef b2::cp_t value_type; 
  typedef b2::cp_t& reference;
  typedef b2::cp_t const& const_reference;
  typedef typename Container::difference_type difference_type;
  typedef size_t size_type;

  // non standard typedefs.
  typedef EncodeTag encode_type;
  typedef Container container_type;
  typedef typename Container::iterator base_iterator;
  typedef typename Container::const_iterator const_base_iterator;

  // type traits struct.
  typedef typename b2::unicode_type_traits<EncodeTag>type_traits;

  // iterators.
  // NOTE! Same typedef for const/non-const iterators since we cannot modify container through iterators.
  typedef const_unicode_iterator<EncodeTag,typename Container::const_iterator>const_iterator;
  typedef boost::reverse_iterator<const_iterator>const_reverse_iterator;
  typedef const_unicode_iterator<EncodeTag,typename Container::const_iterator>iterator;
  typedef boost::reverse_iterator<const_iterator>reverse_iterator;

  // default ctor.
  unicode_container(){}

  // construct from unicode container of same type.
  // (faster than generic copy ctor)
  unicode_container(unicode_container<EncodeTag,Container>const&uc):cont_(uc.cont_){
  }
  // construct from container holding code units.
  // (throws execpton)
  template<typename OtherEncodeTag,typename OtherContainer>
  unicode_container(OtherEncodeTag other_tag,const OtherContainer&uc){
    typedef typename OtherContainer::const_iterator other_iterator;
    BOOST_CONCEPT_ASSERT((boost::InputIterator<other_iterator>));
    std::copy(uc.begin(),uc.end(),std::back_inserter(cont_));
    uni_error uerr;
    if(b2::unicode_validate_encoding<EncodeTag>(uc.begin(),uc.end(),&uerr)!=uc.end())throw uni_exception(uerr);
  }
  // construct from iterators over code units.
  // (throws execpton)
  template<typename OtherEncodeTag,typename InputIt>
  unicode_container(OtherEncodeTag other_tag,InputIt begin,InputIt end){
    BOOST_CONCEPT_ASSERT((boost::InputIterator<InputIt>));
    std::copy(begin,end,std::back_inserter(cont_));
    uni_error uerr;
    if(b2::unicode_validate_encoding<EncodeTag>(begin,end,&uerr)!=end)throw uni_exception(uerr);
  }
  // construct from container holding codepoints.
  // (throws execpton)
  template<typename OtherContainer>
  unicode_container(const OtherContainer&uc){
    typedef typename OtherContainer::const_iterator other_iterator;
    other_iterator cp_it_start(uc.cbegin());
    other_iterator cp_it_end(uc.cend());
    for_each(cp_it_start,cp_it_end,b2::make_unicode_encode<EncodeTag>(std::back_inserter(cont_)));
  }
  // construct from iterators over code points.
  // (throws execpton)
  template<typename InputIt>
  unicode_container(InputIt first,InputIt last){
    BOOST_CONCEPT_ASSERT((boost::InputIterator<InputIt>));
    for_each(first,last,b2::make_unicode_encode<EncodeTag>(std::back_inserter(cont_)));
  }
  // construct from null terminated array of code points.
  // (throws execpton)
  template<typename CP_T>
  unicode_container(CP_T const*cps){
    CP_T const*cp_end=cps;
    while(*cp_end!=0)++cp_end;
    for_each(cps,cp_end,b2::make_unicode_encode<EncodeTag>(std::back_inserter(cont_)));
  }
  // dtor.
  ~unicode_container(){}

  // assignment.
  template<typename OtherEncodeTag,typename OtherContainer>
  unicode_container<EncodeTag,Container>&operator=(const unicode_container<OtherEncodeTag,OtherContainer>&uc){
    unicode_container<EncodeTag>uc_tmp(uc);
    swap(uc_tmp);
    return*this;
  }
  // comparisons.
  bool operator==(const Container&other)const{return cont_.operator==(other);}
  bool operator!=(const Container&other)const{return cont_.operator!=(other);}
  /* For this we need collation - ordering of code points.
     Write this as separate algorithms and use them here.
     Or, implement brute force comparison and have external algorithms for comparisons.
  bool operator<(const X&) const; //optional
  bool operator>(const X&) const; //optional
  bool operator<=(const X&) const; //optional
  bool operator>=(const X&) const; //optional
  */

  // const iterators.
  const_iterator cbegin()const{return const_iterator(cont_.cbegin());}
  const_iterator cend()const{return const_iterator(cont_.cend());}
  const_reverse_iterator crbegin()const{return boost::reverse_iterator<const_iterator>(cend());}
  const_reverse_iterator crend()const{return boost::reverse_iterator<const_iterator>(cbegin());}

  // non-const iterators.
  // NOTE! These iterators are instantiated using non-const base iterators. Not sure if this si correct.
  iterator begin()const{return const_iterator(cont_.begin());}
  iterator end()const{return const_iterator(cont_.end());}
  reverse_iterator rbegin()const{return boost::reverse_iterator<const_iterator>(end());}
  reverse_iterator rend()const{return boost::reverse_iterator<const_iterator>(begin());}

// NOTE! Not sure what these are.
//  const_reverse_iterator rbegin() const; //optional
//  const_reverse_iterator rend() const; //optional

  // get first codepoint in container.
  value_type front()const{return*cbegin();}

  // get last codepoint in container.
  value_type back()const{return*--cend();}

//  template<class ...Args>
//  void emplace_front(Args...); //optional
//  template<class ...Args>
//  void emplace_back(Args...); //optional

  // add a codepoint first in container.
  void push_front(value_type const&cp){
    typename type_traits::cu_t buf[type_traits::max_cu_encode_length];
    typename type_traits::cu_t*cu_ptr=make_unicode_encode<EncodeTag>(&buf[0])(cp);
    while(cu_ptr!=&buf[0])cont_.push_front(*--cu_ptr);
  }
  // add a codepoint at end of container.
  void push_back(value_type const&cp){
    make_unicode_encode<EncodeTag>(std::back_inserter(cont_))(cp);
  }
  // remove first codepoint in container.
  void pop_front(){
    size_t cp_len(unicode_cp_encode_length<EncodeTag>()(front()));
    for(size_t i=0;i<cp_len;++i)cont_.pop_front();
  }
  // remove last codepoint from container.
  void pop_back(){
    size_t cp_len(unicode_cp_encode_length<EncodeTag>()(back()));
    for(size_t i=0;i<cp_len;++i)cont_.pop_back();
  }

// NOTE! Implement acess functions.
//  reference operator[](size_type); //optional
//  const_reference operator[](size_type) const; //optional
//  reference at(size_type); //optional
//  const_reference at(size_type) const; //optional

//  template<class ...Args>
//  iterator emplace(const_iterator, Args...); //optional

  // NOTE! All insert operations are implemented in an inefficient way.
  //       The reason being is that all iterators are invalidated after an insert.
  //       What we should do is 'open up' the container and copy elements into the gap.
  //       This requires a little more thinking and work than the current implementation.
  //
  //       Also, the return value for insert operation should be a non-const iterator.
  //       Since we don't have a unicode non-constant iterator we can't do that.
  //       Question: is it allowed by the standard to have const and non const iterators defined the same way?

  // insert a codepoint before iterator.
  iterator insert(const_iterator position,value_type const&cp){
    typename type_traits::cu_t buf[type_traits::max_cu_encode_length];
    typename type_traits::cu_t*cu_start_ptr=&buf[0];
    typename type_traits::cu_t*cu_end_ptr=make_unicode_encode<EncodeTag>(cu_start_ptr)(cp);
    const_base_iterator cu_it(position.base());
    for(;cu_end_ptr!=cu_start_ptr;)cu_it=cont_.insert(cu_it,*--cu_end_ptr);
    return const_iterator(cu_it);
  }
  // insert the same codepoint multiple times.
  void insert(const_iterator position,size_type n,value_type const&cp){
    for(size_t i=0;i<n;++i)position=insert(position,cp);
  }
  // insert values from a range before - only enable if parameters are input iteratots to avoid overload garbling with
  // insert(iterator,n,cp).
  // NOTE! Should enable only if iterator, not disable - but don't have an is_input_iterator'.
  //       Need this to avoid overload problems with insert(position, n, cp).
  template<typename InputIt>
  void insert(const_iterator position,InputIt first,InputIt last,typename boost::disable_if<boost::is_integral<InputIt>,enabler>::type=enabler()){
    for(;first!=last;++first)position=insert(position++,*first);
  }
// NOTE! For C++0x11
//  iterator insert(const_iterator, std::initializer_list<T>); //optional

  // erase an element.
  iterator erase(const_iterator position){
    const_iterator it_start(position);
    const_iterator it_last(++position);
    typename Container::iterator cu_it(cont_.erase(it_start.base(),it_last.base()));
    return const_iterator(cu_it);
  }
  // erase elements in a range.
  iterator erase(const_iterator first,const_iterator last){
    const_iterator it_start(first);
    const_iterator it_end(last);
    typename Container::iterator cu_it(cont_.erase(first.base(),last.base()));
    return const_iterator(cu_it);
  }
  // clear container.
  void clear(){cont_.clear();}

  // drop all elements in container and assign new ones.
  template<typename InputIt>
  void assign(InputIt first,InputIt last){
    BOOST_CONCEPT_ASSERT((boost::InputIterator<InputIt>));
    unicode_container<EncodeTag,Container>tmp(first,last);
    swap(tmp);
  }
//  void assign(std::initializer_list<T>); //optional

  // drop all elements in container and fill container with n codepoints cp.
  void assign(size_type n,value_type const&cp){
    clear();
    insert(cend(),n,cp);
  }
  // swap two unicode containers.
  void swap(unicode_container<EncodeTag,Container>&uc){cont_.swap(uc.cont_);}

  // get #of codepoints in container.
  size_type size()const{
    return std::distance(const_iterator(cont_.begin()),const_iterator(cont_.end()));
  }
  // get maximum number of codeunits that can be stored.
  size_type max_size()const{return cont_.max_size()/type_traits::max_cu_encode_length;}

  // check if container is empty.
  bool empty()const{return cont_.empty();}

  // get allocator.
  allocator_type get_allocator(){return allocator_type();}

  // get underlying container storing code units.
  container_type const&base_cont()const{return cont_;}
};
/*
template <class T, class A = std::allocator<T> >
std::swap(X<T,A>&, X<T,A>&); //optional
*/

// some typedefs simplifying the code.
typedef unicode_container<b2::utf8_tag,boost::container::deque<b2::unicode_type_traits<b2::utf8_tag>::cu_t> >utf8string;
// NOTE! Typedef for other encodings.
//  ...

}
#endif

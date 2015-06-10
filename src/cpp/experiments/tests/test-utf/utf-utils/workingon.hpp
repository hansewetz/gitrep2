// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

/*
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> START
// NOTE!
// (1) Hide proxy in some namespace or inside iterator.
// (2) Implement operations required by boost::iterator_facade.
// (3) Possibly change template parameters to UnicodeContainer instead (see std::vector)
// (4) Change non-const container functions from taking a const_unicode_iterator to a unicode_iterator
// (5) Change container typedefs to correct iterators (see std::vector)
// (6) Change container to return correct type of iterator (see std::vector)
// (7) Change test1.cc program to use correct iterators for testing


// ---------- (iterator) bidirectional iterator.
// (iterator must have an underlying unicode_container)
// (traverses encoded codepoints and returns one codepoint at a time)
// (if failing to increment the iterator stays at the bad code point and throws an exception)
// (implemented: traversal=bidirectional, element-access=readable/writable)
template<typename EncodeTag,typename Container>class unicode_iterator_value_proxy;
template<typename EncodeTag,typename Container>
class unicode_iterator:public boost::
    iterator_facade<unicode_iterator<EncodeTag,Container>,                     // this class
                    cp_t,                                                               // value type
                    boost::bidirectional_traversal_tag,                                 // traversal strategy
                    unicode_iterator_value_proxy<EncodeTag,Container> >{                // returned when reference is needed
public:
  friend class unicode_iterator_value_proxy<EncodeTag,Container>;
  typedef typename Container::iterator base_iterator;
  typedef unicode_iterator_value_proxy<EncodeTag,Container>proxy;
  typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
  BOOST_CONCEPT_ASSERT((boost::BidirectionalIterator<base_iterator>));

  // ctor.
  explicit unicode_iterator():unicode_cont_(0),cur_(){}
  explicit unicode_iterator(unicode_container<EncodeTag,Container>*unicode_cont,base_iterator cur):unicode_cont_(unicode_cont),cur_(cur){}

  // get underlying iterator.
  base_iterator base()const{return cur_;}
private:
  void increment(){
    difference_type cu_len;
    uni_error::error_code err=unicode_function_traits<EncodeTag,base_iterator>::cu_encode_length(*cur_,cu_len);
    if(err!=uni_error::no_error)throw uni_exception(uni_error(err));
    std::advance(cur_,cu_len);
  }
  void decrement(){
    difference_type cu_len;
    while(unicode_function_traits<EncodeTag,base_iterator>::cu_encode_length(*--cur_,cu_len)==uni_error::error_invalid_lead_byte);
  }
  bool equal(unicode_iterator<EncodeTag,Container>const&other)const{
    return cur_==other.cur_&&unicode_cont_==other.unicode_cont_;
  }
  proxy dereference()const{
    return proxy(*this);
  }
private:
  unicode_container<EncodeTag,Container>*unicode_cont_; // need to know what container we are iterating through.
  base_iterator cur_;                                   // neet to know where we are in base cotainer of unicode_container.
};
// proxy as return value when returning a reference.
template<typename EncodeTag,class Container>
class unicode_iterator_value_proxy{
private:
  typedef unicode_iterator<EncodeTag,Container>iterator;
  typedef typename iterator::base_iterator base_iterator;
  typedef typename std::iterator_traits<base_iterator>::difference_type difference_type;
public:
  // ctors, assignment.
  explicit unicode_iterator_value_proxy(iterator&it):it_(it){}
  explicit unicode_iterator_value_proxy(unicode_iterator_value_proxy<EncodeTag,Container>const&other):it_(other.it_){}
  unicode_iterator_value_proxy&operator=(unicode_iterator_value_proxy const&other){it_=other.it_;}

  // convert to cp_t.
  operator cp_t()const{
    difference_type cu_len;
    cp_t cp;
    uni_error::error_code err=unicode_function_traits<EncodeTag,base_iterator>::decode(it_,cp,static_cast<typename boost::add_pointer<base_iterator>::type>(0),&cu_len);
    if(err!=uni_error::no_error)throw uni_exception(uni_error(err));
    return cp;
  }
  // assign value.
  unicode_iterator_value_proxy<EncodeTag,Container>&operator=(cp_t cp){
    // need to do an erase() + insert() in container.
    // NOTE! Not yet done.
  }
private:
  // keep iterator by value since it may change after user gets hold of the proxy.
  iterator it_;
};
// make a unicode const bidirectional iterator.
template<typename EncodeTag,typename Container>
unicode_iterator<EncodeTag,Container>make_const_unicode_iterator(unicode_container<EncodeTag,Container>&unicode_cont,typename Container::iterator&it){
  return unicode_iterator<EncodeTag,Container>(unicode_cont,it);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> END
*/






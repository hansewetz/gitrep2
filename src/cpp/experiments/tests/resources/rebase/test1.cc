// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "boost/iterator.hpp"
#include <iostream>
#include <vector>
#include <memory>
using namespace std;

template<typename T,typename A=allocator<T>>
class List{
public:
  struct Link;
  using LinkAlloc=typename A:: template rebind<Link>::other;

  // link in list
  struct Link{
    Link(int i):i_(i){}
    int i_;
    Link*next_=0;
  };
  void pushFront(int i){
    // allocate memory and construct object in memory
    Link*mem=linkAlloc_.allocate(sizeof(Link));
    Link*lnk=new(mem)Link(i);

     // insert into list
    if(first==nullptr)first=lnk;
    else{
      lnk->next_=first;
      first=lnk;
    }
  }
  // print list
  void print(ostream&os)const{
    Link*next=first;
    while(next){
      cout<<next->i_<<endl;
      next=next->next_;
    }
  }
private:
  // list state
  Link*first=nullptr;

  // allocators
  LinkAlloc linkAlloc_;
  A listAlloc_;
};

int main(){
  List<int,std::allocator<int>>lst;
  for(int i=0;i<10;++i)lst.pushFront(i);
  lst.print(cout);
}

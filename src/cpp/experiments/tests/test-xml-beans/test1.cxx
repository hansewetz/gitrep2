#include <memory>  
#include <iostream>
#include "hello.hxx"
using namespace std;

// test program
int main(){
  try{
    auto_ptr<hello_t>h(hello("hello.xml"));
    cout<<"greeting: "<<h->greeting()<<endl;
    for(hello_t::name_const_iterator it(h->name().begin());it!=h->name().end();++it)
      cout<<"name: "<<*it<<endl;
  }
  catch (const xml_schema::exception&e){
    cerr<<e<<endl;
    return 1;
  }
}

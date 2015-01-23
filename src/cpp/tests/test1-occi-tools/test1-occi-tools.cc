// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "occi-tools/occi_tools.h"
#include <occiData.h>
#include <iostream>
#include <memory>
#include <tuple>
#include <stdexcept>
#include <algorithm>
using namespace std;
using namespace occi_utils;
using namespace utils;
using namespace oracle::occi;

// --- test insert
void update(occi_auth const&auth){
  typedef tuple<int,string>bind_t;
  typedef tuple<int,int>size_t;
  string sql{"insert into junk (id,name,isrt_tmstmp) values(:1,:2,sysdate)"};
//  occi_sink<bind_t>sink(auth,sql,3,size_t{0,100},occi_sink<bind_t>::commit_t::Commit);
  occi_sink<bind_t>sink(auth,sql,2,size_t(0,100));
  vector<bind_t>b{bind_t{1,"hans"},bind_t{2,"ingvar"},bind_t{3,"valdemar"}};
  auto it=sink.begin();
  copy(b.begin(),b.end(),it);
  it.flush();
/*
  string sql{"delete from junk"};
  occi_sink<tuple<>>sink(auth,sql);
  *sink.begin()=tuple<>();
*/
}
// --- test select
void select(occi_auth const&auth){
  // sql statement, row and bind types, bind data
  typedef tuple<int,string,string,string>row_t;
  typedef tuple<string>bind_t;
  string sql{"select rownum,dbid,matid,languagecode from mt_tmlo where dbid=:1 and rownum<10"};
  bind_t bind{"Policy"};

  // read from input collection
  occi_source<row_t,bind_t>rows{auth,sql,100,bind};
  for(auto r:rows)cout<<r<<endl;
}
void select1(occi_auth const&auth){
  // sql statement, row and bind types, bind data
  typedef tuple<string>row_t;
  string sql{"select matid from (select matid from mt_tmlo where changedate = to_date('15-MAY-13', 'DD-MON-YY') order by matid desc) where rownum < 2"};

  // read from input collection
  occi_source<row_t>rows{auth,sql,10};
  for(auto r:rows)cout<<r<<endl;
}
// --- main test program
int main(){
  // auth credentials
  //occi_auth const auth{"user","passwd","mydb"};
  occi_auth const auth{"sdt02","rjszk#JDKE4239prmsd","MTDADGTP"};
  try {
    select(auth);
    //update(auth);
  }
  catch(std::exception&e){
    cerr<<"caught exception: "<<e.what()<<endl;
    return 1;
  }
  return 0;
}

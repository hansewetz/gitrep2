#include "occi_tools.h"
#include <occiData.h>
#include <iostream>
#include <memory>
#include <tuple>
#include <stdexcept>
#include <algorithm>
using namespace std;
using namespace mtdata;
using namespace oracle::occi;

// --- test insert
void update(occi_auth const&auth){
  typedef tuple<int,string>bind_t;
  typedef tuple<int,int>size_t;
  string sql{"insert into junk (id,name,isrt_tmstmp) values(:1,:2,sysdate)"};
  occi_sink<bind_t>sink(auth,sql,3,size_t{0,100},occi_sink<bind_t>::commit_t::Commit);
  vector<bind_t>b{bind_t{1,"hans"},bind_t{2,"ewetz"},bind_t{3,"claes"}};
  copy(b.begin(),b.end(),sink.begin());
}
// --- test select
void select(occi_auth const&auth){
  // sql statement, row and bind types, bind data
  typedef tuple<int,string,string,string>row_t;
  typedef tuple<string>bind_t;
  string sql{"select rownum,dbid,tab,cutoff_dt from MT_DBSYNC_CUTOFF_DT where dbid=:1"};
  bind_t bind{"Policy"};

  // read from input collection
  occi_source<row_t,bind_t>rows{auth,sql,100,bind};
  for(auto r:rows)cout<<r<<endl;
}
// --- main test program
int main(){
  // auth credentials
  occi_auth const auth{"sdt02","rjszk#JDKE4239prmsd","MTECDGTD"};
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

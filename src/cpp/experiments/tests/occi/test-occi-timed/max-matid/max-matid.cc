// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "oci_deleter.h"
#include <iostream>
#include <fstream>
#include <ratio>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace oracle::occi;

// convert base36 number to decimal - must be 6 chars long and must be correct
unsigned long convertFromBase36(string const&s){
  static unsigned long posval[6]{36*36*36*36*36,36*36*36*36,36*36*36,36*36,36,1};
  unsigned long ret=0;
  for(int i=0;i<6;++i){
    if(s[i]>'9')ret+=(s[i]-'a')*posval[i];
    else ret+=(s[i]-'0')*posval[i];
  }
  return ret;
}
// main program.
int main (int argc, char* argv[]){
  // db config parameters
  const int NROWS=800*std::mega::num;
  const int NPREFETCH=10000;

  // db connection parameters
  const string user("mtecal2");
  const string pass("bhiskwchxy1$");
  const string osid("MTECDGTD");

  //std::shared_ptr<Environment>env(Environment::createEnvironment(Environment::DEFAULT),occi_env_deleter());
  std::shared_ptr<Environment>env(Environment::createEnvironment("UTF8","UTF8"),occi_env_deleter());
  int ret=0;
  try {
    // create connection.
    cout<<"Connecting as "<<user<<"@"<<osid<<"."<< endl;
    std::shared_ptr<Connection>conn(std::shared_ptr<Connection>(env->createConnection(user,pass,osid),occi_conn_deleter(env.get())));
    cout<<"Connected as "<<user<<"@"<<osid<<"."<< endl;

    // setup statement for query with a prefetch size of 10Ki (seems to be kind of optimal).
    string sql{string("select matid from MT_TMMAT where rownum <= ")+boost::lexical_cast<string>(NROWS)};
    std::shared_ptr<Statement>stmt(conn->createStatement(sql.c_str()),occi_stmt_deleter(conn.get()));
    stmt->setPrefetchRowCount(NPREFETCH);

    // execute query.
    std::shared_ptr<ResultSet>rs(stmt->executeQuery(),occi_rs_deleter(stmt.get()));

    // get data
    unsigned long maxMatid=0;
    int count=0;
    while(rs->next()){
      ++count;
      if((count%100000)==0){
        cout<<"count: "<<count/1000<<"K"<<'\r'<<flush;
      }
      unsigned long dmatid = convertFromBase36(rs->getString(1));
      if(dmatid>maxMatid)maxMatid=dmatid;
    }
    // print info
    cout<<endl<<"count: "<<count<<", max matid: "<<maxMatid<<endl;
  }
  catch (SQLException e) {
    cerr<<"oracle error: "<<e.what();
    ret=1;
  }
  return ret;
}

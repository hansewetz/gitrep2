// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "oci_deleter.h"
#include <iostream>
#include <fstream>
#include <ratio>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace oracle::occi;

// print date
ostream&operator<<(ostream&os,oracle::occi::Date const&d){
  return os<<(d.isNull()?"":d.toText());
}

// decide if selecting date column
#define USE_CDATE

// main program.
int main (int argc, char* argv[]){
  // db array/prefetching parameters
  const int NARRAYFETCH=100*kilo::num;
  const int NPREFETCH=100*kilo::num;
//NOTE!  const int NROWS=800*mega::num;
  const int NROWS=10;

  // output file
  ofstream os("junk.txt");

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
#ifdef USE_CDATE
    string sql{string("select matid,languagecode,changedate,dbid from MT_TMLO where changedate >= to_date('19-Feb-13','DD-Mon-YY') AND rownum <= ")+boost::lexical_cast<string>(NROWS)};
#else
    string sql{string("select matid,languagecode from MT_TMLO where rownum <= ")+boost::lexical_cast<string>(NROWS)};
#endif
    std::shared_ptr<Statement>stmt(conn->createStatement(sql.c_str()),occi_stmt_deleter(conn.get()));
    stmt->setPrefetchRowCount(NPREFETCH);

    // execute query.
    std::shared_ptr<ResultSet>rs(stmt->executeQuery(),occi_rs_deleter(stmt.get()));

    // setup matid buffer
    const int matidSize=7;
    char bufMatid[NARRAYFETCH][matidSize];
    rs->setDataBuffer(1,&bufMatid[0],OCCI_SQLT_STR,matidSize);

    // setup languagecode buffer
    const int lanSize=3;
    char bufLan[NARRAYFETCH][lanSize];
    rs->setDataBuffer(2,&bufLan[0],OCCI_SQLT_STR,lanSize);

    // setup change date buffer
#ifdef USE_CDATE
    oracle::occi::Date bufCdate[NARRAYFETCH];
    rs->setDataBuffer(3,&bufCdate[0],OCCI_SQLT_DAT,sizeof(oracle::occi::Date));
#endif    
    // setup dbid buffer
    const int dbidSize=8;
    char bufDbid[NARRAYFETCH][dbidSize];
    rs->setDataBuffer(4,&bufDbid[0],OCCI_SQLT_STR,dbidSize);

    int count=0;
    while(rs->next(NARRAYFETCH)!=ResultSet::END_OF_FETCH){
      int nrows=rs->getNumArrayRows();
      for(int i=0;i<nrows;++i){
        ++count;
        if((count%100000)==0){
          cout<<"count: "<<count/1000<<"K"<<'\r'<<flush;
        }
#ifdef USE_CDATE
        os<<bufMatid[i]<<"|"<<bufLan[i]<<"|"<<bufCdate[i]<<"|"<<bufDbid[i]<<endl;
#else
        os<<bufMatid[i]<<"|"<<bufLan[i]<<endl;
#endif
      }
    }
    cout<<endl<<"count: "<<count<<endl;
  }
  catch (SQLException e) {
    os.close();
    cerr<<"oracle error: "<<e.what();
    ret=1;
  }
  os.close();
  return ret;
}

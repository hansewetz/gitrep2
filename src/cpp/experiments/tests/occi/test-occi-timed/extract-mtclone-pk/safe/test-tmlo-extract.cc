// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include "oci_deleter.h"
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace oracle::occi;

// print date
ostream&operator<<(ostream&os,Date const&d){
  return os<<(d.isNull()?"":d.toText());
}

// main program.
int main (int argc, char* argv[]){
  // db array/prefetching parameters
  const int NPREFETCH=10000;
  const int NROWS=1000000;

  // output file
  ofstream os("junk.txt");

  // db connection parameters
  const string user("mtecal2");
  const string pass("bhiskwchxy1$");
  const string osid("MTECDGTD");

  // to store data in
  class PkDate{
  public:
    PkDate(string const&matid,string const&languagecode,Date const&d):
      matid_(matid),languagecode_(languagecode),d_(d){
    }
  private:
    string matid_;
    string languagecode_;
    Date d_;
  };
  vector<PkDate>pkdates;
//  pkdates.reserve(NROWS);

  //std::shared_ptr<Environment>env(Environment::createEnvironment(Environment::DEFAULT),occi_env_deleter());
  std::shared_ptr<Environment>env(Environment::createEnvironment("UTF8","UTF8"),occi_env_deleter());
  int ret=0;
  try {
    // create connection.
    cout<<"Connecting as "<<user<<"@"<<osid<<"."<< endl;
    std::shared_ptr<Connection>conn(std::shared_ptr<Connection>(env->createConnection(user,pass,osid),occi_conn_deleter(env.get())));
    cout<<"Connected as "<<user<<"@"<<osid<<"."<< endl;

    // setup statement for query with a prefetch size of 10Ki (seems to be kind of optimal).
    string sql{string("select matid,languagecode,changedate from MT_TMLO where rownum <= ")+boost::lexical_cast<string>(NROWS)};
    std::shared_ptr<Statement>stmt(conn->createStatement(sql.c_str()),occi_stmt_deleter(conn.get()));
    stmt->setPrefetchRowCount(NPREFETCH);

    // execute query.
    std::shared_ptr<ResultSet>rs(stmt->executeQuery(),occi_rs_deleter(stmt.get()));
    
    int count=0;
    while(rs->next()){
      ++count;
      if((count%100000)==0){
        cout<<"count: "<<count/1000<<"K"<<'\r'<<flush;
      }
//      pkdates.push_back(PkDate(rs->getString(1),rs->getString(2),rs->getDate(3)));
//      os<<rs->getString(1)<<"|"<<rs->getString(2)<<"|"<<rs->getString(3)<<endl;
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

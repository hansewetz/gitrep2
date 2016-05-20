// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include "oci_deleter.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace oracle::occi;

// print date
ostream&operator<<(ostream&os,Date const&d){
  return os<<(d.isNull()?"":d.toText());
}

// main program.
int main (int argc, char* argv[]){
  ofstream os("junk.txt");

  const string user("sdt02");
  const string pass("sdt02");
  const string osid("EURASDTD");

  //std::shared_ptr<Environment>env(Environment::createEnvironment(Environment::DEFAULT),occi_env_deleter());
  std::shared_ptr<Environment>env(Environment::createEnvironment("UTF8","UTF8"),occi_env_deleter());
  int ret=0;
  try {
    // create connection.
    cout<<"Connecting as "<<user<<"@"<<osid<<"."<< endl;
    std::shared_ptr<Connection>conn(std::shared_ptr<Connection>(env->createConnection(user,pass,osid),occi_conn_deleter(env.get())));
    cout<<"Connected as "<<user<<"@"<<osid<<"."<< endl;

    // setup statement for query with a prefetch size of 10Ki (seems to be kind of optimal).
    string sql{"select "
              " dbid,matid,languagecode,storeddate,storedby,mainentry,creationdate,creationuser,translator,changedate,changeuser,changetranslator,exist_segment_en "
              " from tmlo where dbid = 'Policy' and changedate > '15-APR-13'"};
    std::shared_ptr<Statement>stmt(conn->createStatement(sql.c_str()),occi_stmt_deleter(conn.get()));
    stmt->setPrefetchRowCount(10000);

    // execute query.
    std::shared_ptr<ResultSet>rs(stmt->executeQuery(),occi_rs_deleter(stmt.get()));
    
    int count=0;
    while(rs&&rs->next()){
      ++count;
      if((count%100000)==0){
        cout<<"count: "<<count/1000<<"K"<<'\r'<<flush;
      }
      os<<rs->getString(1)<<"|"<<rs->getString(2)<<"|"<<rs->getString(3)<<"|"<<rs->getDate(4)<<"|"<<rs->getString(5)<<"|"<<rs->getString(6)<<"|"<<rs->getDate(7)<<"|"<<
          rs->getString(8)<<"|"<<rs->getString(9)<<"|"<<rs->getDate(10)<<"|"<<rs->getString(11)<<endl;
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

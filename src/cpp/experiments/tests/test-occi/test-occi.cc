#include "occi_deleter.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

using namespace std;
using namespace oracle::occi;

// main program.
int main (int argc, char* argv[]){
  const string user("sdt02");
  const string pass("sdt02");
  const string osid("EURASDTD");

  std::shared_ptr<Environment>env(Environment::createEnvironment(Environment::DEFAULT),occi_env_deleter());
  int ret=0;
  try {
    // create connection.
    cout<<"Connecting as "<<user<<"@"<<osid<<"."<< endl;
    std::shared_ptr<Connection>conn(std::shared_ptr<Connection>(env->createConnection(user,pass,osid),occi_conn_deleter(env.get())));
    cout<<"Connected as "<<user<<"@"<<osid<<"."<< endl;

    // setup statement for query.
    std::shared_ptr<Statement>stmt(conn->createStatement("select count(1), dbid from tmmat where rownum<10 group by dbid"),occi_stmt_deleter(conn.get()));

    // execute query.
    std::shared_ptr<ResultSet>rs(stmt->executeQuery(),occi_rs_deleter(stmt.get()));
    
    if(rs){
      // get all dbid's from tmmat.
      typedef pair<int,string>count_t;
      vector<count_t>dbid_counts;
      while(rs->next())dbid_counts.push_back(make_pair(rs->getInt(1),rs->getString(2)));

      // sort result on count(1) in desc order and print it.
      sort(dbid_counts.begin(),dbid_counts.end(),std::bind(less<int>(),std::bind(&count_t::first,std::placeholders::_1),std::bind(&count_t::first,std::placeholders::_2)));
      cout<<"COUNT(1)\tDBID"<<endl<<"=================="<<endl;
      for(vector<count_t>::const_iterator it=dbid_counts.begin();it!=dbid_counts.end();++it){
        cout<<it->first<<"\t"<<it->second<<endl;
      }
    }
  }
  catch (SQLException e) {
    cerr<<"oracle error: "<<e.what();
    ret=1;
  }
  return ret;
}

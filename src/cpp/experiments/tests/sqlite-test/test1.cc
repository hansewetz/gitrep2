// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <string>
#include <iostream>
#include "sqlite_modern_cpp.h"

using namespace  sqlite;
using namespace std;

// main test program
int main(){
  // creates a database file 'dbfile.db' if it does not exists.
  database db("test.db");

  // select all parameters + executable for a given service
  string service{"dummy1-serv"};
  db << "select t1.name, t4.name, t3.value, t3.descr from service_definition t1 "
        "join serv_param_intersect t2 on (t1.name = t2.serv_name) "
        "join service_parameter t3 on (t2.param_id = t3.id) "
        "join executable t4 on (t1.executable = t4.name) "
        "where t1.name = ?;"
      <<service
      >>[&](string serv,string exe,string param,string descr){
    cout<<"service: "<<serv<<", exe: "<<exe<<", param: "<<param<<", descr: "<<descr<<endl;
  };
  // count #of service in schema
  int count;
  db<<"select count(1) from service_definition">>count;
  cout<<"#services: "<<count<<endl;
}

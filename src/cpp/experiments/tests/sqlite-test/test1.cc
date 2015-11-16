#include <string>
#include <iostream>
#include "sqlite_modern_cpp.h"

using namespace  sqlite;
using namespace std;

// main test program
int main(){
  // service to select
  string service{"dummy1-serv"};

  // creates a database file 'dbfile.db' if it does not exists.
  database db("junk.db");

  // executes the query and creates a 'user' table
  db <<"create table if not exists user (id integer primary key autoincrement not null,"
       "   age int, name text, weight real);";  

  db << "select t1.name, t4.name, t3.value, t3.descr from service_definition t1 "
        "join serv_param_intersect t2 on (t1.name = t2.serv_name) "
        "join service_parameter t3 on (t2.param_id = t3.id) "
        "join executable t4 on (t1.executable = t4.name) "
        "where t1.name = ?;"
      <<service
      >>[&](string serv,string exe,string param,string descr){
    cout<<"service: "<<serv<<", exe: "<<exe<<", param: "<<param<<", descr: "<<descr<<endl;
  };
}
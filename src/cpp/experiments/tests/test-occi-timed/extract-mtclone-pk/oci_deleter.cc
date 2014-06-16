#include "oci_deleter.h"
using namespace oracle::occi;

// deleter for Environment.
void occi_env_deleter::operator()(oracle::occi::Environment*env)const{
  if(env)oracle::occi::Environment::terminateEnvironment(env);
}
// deleter for connection.
occi_conn_deleter::occi_conn_deleter(oracle::occi::Environment*env):env_(env){}
void occi_conn_deleter::operator()(oracle::occi::Connection*conn)const{
  if(env_&&conn)env_->terminateConnection(conn);
}
// deleter for statement.
occi_stmt_deleter::occi_stmt_deleter(oracle::occi::Connection*conn):conn_(conn){}
void occi_stmt_deleter::operator()(oracle::occi::Statement*stmt)const{
  if(conn_&&stmt)conn_->terminateStatement(stmt);
}
// deleter for result set.
occi_rs_deleter::occi_rs_deleter(oracle::occi::Statement*stmt):stmt_(stmt){}
void occi_rs_deleter::operator()(oracle::occi::ResultSet*rs)const{
  if(stmt_&&rs)stmt_->closeResultSet(rs);
}

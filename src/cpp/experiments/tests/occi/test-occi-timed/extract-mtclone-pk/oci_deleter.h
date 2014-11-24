// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#ifndef OCI_DELETER_H
#define OCI_DELETER_H
#include <occiData.h>
#include <memory>

// deleter for Environment.
struct occi_env_deleter{
  void operator()(oracle::occi::Environment*env)const;
};
// deleter for connection.
class occi_conn_deleter{
public:
  explicit occi_conn_deleter(oracle::occi::Environment*env);
  void operator()(oracle::occi::Connection*conn)const;
private:
  oracle::occi::Environment*env_;
};
// deleter for statement.
class occi_stmt_deleter{
public:
  explicit occi_stmt_deleter(oracle::occi::Connection*conn);
  void operator()(oracle::occi::Statement*stmt)const;
private:
  oracle::occi::Connection*conn_;
};
// deleter for result set.
class occi_rs_deleter{
public:
  explicit occi_rs_deleter(oracle::occi::Statement*stmt);
  void operator()(oracle::occi::ResultSet*rs)const;
private:
  oracle::occi::Statement*stmt_;
};
#endif

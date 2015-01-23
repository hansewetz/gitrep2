// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include <iostream>
#include <sqlite3.h>

using namespace std;

typedef int (*sqlite3_callback)(
  void*,    /* Data provided in the 4th argument of sqlite3_exec() */
  int,      /* The number of columns in row */
  char**,   /* An array of strings representing fields in the row */
  char**    /* An array of strings representing column names */
);

static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int main(){
  const char*dbname="dbjunk";

  sqlite3*db;
  char*zErrMsg=0;
  int rc;

  rc=sqlite3_open(dbname,&db);
  if(rc){
    fprintf(stderr,"Can't open database: %s\n",sqlite3_errmsg(db));
    exit(0);
  }else{
    fprintf(stderr,"Opened database successfully\n");
  }
  const char*sql = "SELECT * from JUNK";
  const char* data = "Callback function called";
  rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    fprintf(stdout, "Operation done successfully\n");
  }
  sqlite3_close(db);
}

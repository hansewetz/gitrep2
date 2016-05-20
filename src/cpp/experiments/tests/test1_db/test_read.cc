// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <db_cxx.h>
using namespace std;

int main(){
  u_int32_t oFlags=DB_RDONLY;
  Db db(NULL,0);
  try{
    db.open(NULL,"test.db",NULL,DB_BTREE,oFlags,0);

    int my_key=1;
    Dbt key;
    key.set_data(&my_key);
    key.set_size(sizeof(my_key));
    
    Dbt data;
    char my_data[12];
    data.set_data(my_data);
    data.set_ulen(12);
    data.set_flags(DB_DBT_USERMEM);

    int ret=db.get(NULL,&key,&data,0);
    if(ret==DB_NOTFOUND){
      cerr<<"Nothing found for key: "<<my_key<<endl;
    }else{
      cerr<<"Got data: \""<<my_data<<"\""<<endl;
      db.close(0);
    }
  }
  catch(DbException &e){
    cerr<<"DB exception: "<<e.what()<<endl;
    return 1;
  }
  catch(exception&e){
    cerr<<"std exception: "<<e.what()<<endl;
    return 1;
  }
  catch(...){
    cerr<<"unknownnown error"<<endl;
    return 1;
  }
  return 0;
}

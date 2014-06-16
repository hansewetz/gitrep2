#include <iostream>
#include <string.h>
#include <db_cxx.h>
using namespace std;

int main(){
  u_int32_t oFlags=DB_CREATE;
  Db db(NULL,0);
  try{
    db.open(NULL,"test.db",NULL,DB_BTREE,oFlags,0);

    int my_key=1;
    char*my_data=const_cast<char*>("Hello there");
    Dbt key(&my_key,sizeof(my_key));
    Dbt data(my_data,strlen(my_data)+1);
    int ret=db.put(NULL,&key,&data,DB_NOOVERWRITE);
    cerr<<"ret: "<<ret<<endl;
    db.close(0);
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

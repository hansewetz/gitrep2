// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "Poco/Net/FTPClientSession.h"
#include <iostream>
#include <stdlib.h>
using namespace std;
using namespace Poco;
using namespace Poco::Net;

// main test program
int main(){
  try{
    // conection information
    string host{"euramisd.cc.cec.eu.int"};
    int port{21};
    string user{"ewetzha"};
    string passwd{"murry91."};

    // connect, login and close session
    FTPClientSession ftpclient(host,port);
    cerr<<"connected ..."<<endl;
    ftpclient.login(user,passwd);
    cerr<<"logged in"<<endl;
    ftpclient.close();
    cerr<<"session closed"<<endl;
  }
  catch(exception&e){
    cerr<<"exception: "<<e.what()<<endl;
    exit(1);
  }
}

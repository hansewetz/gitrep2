// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#pragma GCC diagnostic ignored "-Wpedantic"
#include <libssh/sftp.h>
#pragma GCC diagnostic pop
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <stdlib.h>
#include <stdexcept>
#include <memory>
using namespace std;

// close ssh session
struct ssh_del{
  void operator()(ssh_session sess){
    ssh_disconnect(sess);
    ssh_free(sess);
  }
};
// close ftp session
struct sftp_del{
  void operator()(sftp_session sftp){
    sftp_free(sftp);
  }
};
// main test program
int main(){
  // conection information
  string host{"achird.cc.cec.eu.int"};
  string user{"mtdata"};
  string passwd{"Hempty2data"};

  try{
    // setup ssh session
    int rc;
    shared_ptr<ssh_session_struct>sess{ssh_new(),ssh_del()};
    if(!sess)throw std::runtime_error("could not create new ssh session");
    ssh_options_set(sess.get(), SSH_OPTIONS_HOST,host.c_str());
    ssh_options_set(sess.get(), SSH_OPTIONS_USER,user.c_str());
    if((rc = ssh_connect(sess.get()))!=SSH_OK)throw std::runtime_error("could not connect ssh session");
    if((rc = ssh_userauth_password(sess.get(), NULL, passwd.c_str()))!=SSH_AUTH_SUCCESS)throw std::runtime_error("ssh session authentication failed");
    
    // create sftp session
    shared_ptr<sftp_session_struct>sftp{sftp_new(sess.get())};
    if(!sftp)throw std::runtime_error("could not create new sftp session");
    if((rc = sftp_init(sftp.get()))!=SSH_OK)throw std::runtime_error("could not init sftp session");

    // list directory
    sftp_dir dir;
    if(!(dir = sftp_opendir(sftp.get(), "/ec/dgt/local/exodus/user/mtdata/junk")))throw std::runtime_error("could open directory");
    sftp_attributes attributes;
    while ((attributes = sftp_readdir(sftp.get(), dir)) != NULL){
      cerr<<">>> "<<attributes->name<<endl;
    }
    if (!sftp_dir_eof(dir))throw std::runtime_error("error while reading directory");
    if((rc = sftp_closedir(dir))!=SSH_OK)throw std::runtime_error("error while closing directory");
  }
  catch(exception&e){
    cerr<<"exception: "<<e.what()<<endl;
    exit(1);
  }
}

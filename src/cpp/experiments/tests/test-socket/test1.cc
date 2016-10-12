#include <iostream>
#include <thread>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
using namespace std;

// server thread
void server(int port){
  // create socket
  int fdserv=socket(AF_INET,SOCK_STREAM,0);

  // bind socket to address
  struct sockaddr_in servaddr;
  memset(&servaddr,'0',sizeof(servaddr));
  servaddr.sin_family=AF_INET;
  servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
  servaddr.sin_port=htons(port);
  if(bind(fdserv,(struct sockaddr*)&servaddr,sizeof(servaddr))){
    perror("bind failed");
    return;
  }
  // start listening on socket
  if(listen(fdserv,10)<0){
    perror("listen failed");
    return;
  }
  while(true){
    // setup file descriptors to read from
    fd_set readfs;
    FD_ZERO(&readfs);
    FD_SET(fdserv,&readfs);
    int maxfd=fdserv;

    // setup timeout
    struct timeval tmval;
    tmval.tv_sec=5;
    tmval.tv_usec=0;

    // wait for client connection or timeout
    int stat=select(maxfd+1,&readfs,nullptr,nullptr,&tmval);
    if(FD_ISSET(fdserv,&readfs)){
      int fdconn=accept(fdserv,nullptr,nullptr);
      char const*msg="Hello";
      write(fdconn,msg,strlen(msg)+1);
      close(fdconn);
    }else
    if(stat<0){
      break;
    }else{
      break;
    }
  }
  close(fdserv);
}
// client calling server
void client(string const&addr,int port){
  // create client socket
  int fdclient=socket(AF_INET,SOCK_STREAM,0);

  // setup address
  struct sockaddr_in servaddr;
  memset(&servaddr,'0',sizeof(servaddr));
  servaddr.sin_family=AF_INET;
  servaddr.sin_port=htons(port);
  if(inet_pton(AF_INET,addr.c_str(),&servaddr.sin_addr)<0){
    perror("address resolution failed");
    return;
  }
  // connect to server
  if(connect(fdclient,(struct sockaddr*)&servaddr,sizeof(servaddr))<0){
    perror("connect failed");
    return;
  }
  // read from server
  char buf[4096];
  int n;
  while((n=read(fdclient,buf,sizeof(buf)-1))>0){
    buf[n]=0;
    cout<<buf;
  }
  cout<<endl;

  // close socket
  close(fdclient);
}

// test program -------------------

// test program
int main(){
  try{
    int port=5001;
    string servaddr="127.0.0.1";
    thread thrserver{server,port};

    // run client in main thread
    for(int i=0;i<10;++i){
      sleep(1);
      client(servaddr,port);
    }
    // join server thread
    thrserver.join();
  }catch(...){
    cout<<"exception"<<endl;
  }
}

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
using namespace std;

// print error  with error code
void errsys(string const&str){
  cerr<<str<<", errno: "<<errno<<", \""<<strerror(errno)<<"\""<<endl;
  exit(1);
}
// write n bytes
int writen(int fd,char*buf,int nbuf){
  int nleft=nbuf;
  while(nleft>0){
    int nwritten=write(fd,buf,nleft);
    if(nwritten<=0)errsys("could not write to fd");
    buf+=nwritten;
    nleft-=nwritten;
  }
  return nbuf-nleft;
}
// create pair of unnamed connected pipes
int streamPipe(int fd[2]){
  return socketpair(AF_UNIX,SOCK_STREAM,0,fd);
}
// run filter process
void filter(int fd){
  fd_set fdread;

  // loop forever and filter between tty and shell
  while(true){
    const size_t BUFSIZE=1024;
    char buf[BUFSIZE];
    int nread;
    int nwritten;

    FD_ZERO(&fdread);
    FD_SET(0,&fdread);
    FD_SET(fd,&fdread);
    int n=select(fd+1,&fdread,0,0,0);
    if(n<=0)errsys("select failed");
    if(FD_ISSET(0,&fdread)){
      if((nread=read(0,buf,BUFSIZE))<0)errsys("failed reading from stdin"); else
      if(nread==0)break; // EOF
      if(writen(fd,buf,nread)!=nread)errsys("failed writing to shell");
    }
    if(FD_ISSET(fd,&fdread)){
      // read from pipe and write to tty (stdout)
      if((nread=read(fd,buf,BUFSIZE))<=0)break;
      if((nwritten=write(1,buf,nread))<0)errsys("could not write to stdout");
    }
  }
  exit(0);
}
// run shell
void shell(int fd,char*argv[],char*env[]){
  // close and dup stdin etc.
  close(0);
  close(1);
  close(2);
  if(dup(fd)!=0||dup(fd)!=1||dup(fd)!=2)errsys("could not dup fds");

  // get shell and run it
  char*shell=getenv("SHELL");
  if(shell==0)errsys("could not get shell to run");
  if(execve(shell,argv,env)!=0)errsys("could not execute shell");
}
// create a pipe between forked shell and this program (filter)
int main(int argc,char*argv[],char*env[]){
  // make sure we are at a terminal
  if(!isatty(0)||!isatty(1))errsys("stdin or stdout is not a terminal");

  // create stream pipe
  int fd[2];
  if(streamPipe(fd)!=0)errsys("could not create stream pipe");

  // fork child process which will be the shell
  int childpid=fork();
  if(childpid<0)errsys("could not fork");
  if(childpid==0){
    close(fd[0]);
    shell(fd[1],argv,env);
  }
  close(fd[1]);
  filter(fd[0]);
  exit(0);
}

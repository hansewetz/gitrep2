// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include "utils/sysUtils.h"
#include "utils/utility.h"
#include <cstring>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/prctl.h>

using namespace std;
namespace utils{

// close a file descriptor
int eclose(int fd,bool throwExcept){
  while(close(fd)<0&&errno==EINTR);
  if(errno&&throwExcept){
    string err{strerror(errno)};
    THROW_RUNTIME("eclose: failed closing fd: "<<err);
  }
  return errno;
}
// dup an fd
int edup(int fd){
  int ret{dup(fd)};
  if(ret<0){
    string err{strerror(errno)};
    THROW_RUNTIME("edup: failed dup() call: "<<err);
  }
  return ret;
}
// set fd to non-blocking
void setFdNonblock(int fd){
  int flags = fcntl(fd,F_GETFL,0);
  if(fcntl(fd, F_SETFL,flags|O_NONBLOCK)<0){
    string err{strerror(errno)};
    THROW_RUNTIME("setFdNonblock: failed setting fd in non-blocking mode: "<<err);
  }
}
// spawn child process setting up stdout and stdin as a pipe
int spawnPipeChild(string const&file,vector<string>args,int&fdRead,int&fdWrite,bool dieWhenParentDies,string const&childdir){
  // create pipe between child and parent
  int fromChild[2];
  int toChild[2];
  if(pipe(toChild)!=0||pipe(fromChild)!=0){
    string err{strerror(errno)};
    THROW_RUNTIME("spawnPipeChild: failed creating pipe: "<<err);
  }
  // set parent read side to non-blocking
  setFdNonblock(fromChild[0]);

  // fork child process
  int pid=fork();
  if(pid==0){ // child
    // change directory
    if(chdir(childdir.c_str())!=0){
      string err{strerror(errno)};
      THROW_RUNTIME("spawnPipeChild: failed executing chdir: "<<err);
    }
    // die if parent dies so we won't become a zombie
    if(dieWhenParentDies&&prctl(PR_SET_PDEATHSIG,SIGHUP)<0){
      string err{strerror(errno)};
      THROW_RUNTIME("spawnPipeChild: failed call to prctl(...): "<<err);
    }
    // dup stdin/stdout ---> pipe, and close original pipe fds
    close(0);close(1);
    edup(toChild[0]);edup(fromChild[1]);
    close(toChild[1]);close(fromChild[0]);
    close(toChild[0]);close(fromChild[1]);

    // setup for calling execv 
    // (we don't care if we leak memory since we'll overlay process using exec)
    char*const*tmpargs=(char*const*)malloc((args.size()+1)*sizeof(char*const*));
    if(tmpargs==0){
      string err{strerror(errno)};
      THROW_RUNTIME("spawnPipeChild: failed call to malloc(...): "<<err);
    }
    char**ptmpargs=const_cast<char**>(tmpargs);
    for(std::size_t i=0;i<args.size();++i)ptmpargs[i]=const_cast<char*>(args[i].c_str());
    ptmpargs[args.size()]=0;

    // execute child process
    if(execvp(file.c_str(),tmpargs)<0){
      string err{strerror(errno)};
      THROW_RUNTIME("spawnPipeChild: failed executing execl: "<<err);
    }else{
      // should never get here
      string err{strerror(errno)};
      THROW_RUNTIME("spawnPipeChild: failed executing execl: "<<err);
    }
  }else
  if(pid>0){ // parent
    // close fds we don't use
    close(fromChild[1]);close(toChild[0]);

    // set return parameters and return child pid
    fdRead=fromChild[0];
    fdWrite=toChild[1];
    return pid;
  }else{
    // fork failed
    string err{strerror(errno)};
    THROW_RUNTIME("spawnPipeChild: failed fork: "<<err);
  }
}
}

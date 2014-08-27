/*
problem is that 'async_read(...)' reads until buffer is full.
right now, buffer size is set to 1.
should instead use 'async_read_until(...) or something else
*/
#include <cstring>
#include <string>
#include <vector>
#include <array>
#include <functional>
#include <iterator>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/asio.hpp>
using namespace std;
using namespace std::placeholders;
namespace asio=boost::asio;

// throw macro for exceptions
#define THROW_RUNTIME(x){\
  std::stringstream strm;\
  strm<<__FILE__":#"<<__LINE__<<": "<<x;\
    std::string s{strm.str()};\
    throw std::runtime_error(s);\
}
// callback handler for read
void read_handler(boost::system::error_code const&err,size_t nbytes,asio::posix::stream_descriptor*ais,asio::streambuf*pbuf){
  if(nbytes!=0){
    istreambuf_iterator<char>end;
    istreambuf_iterator<char>it{pbuf};
    for(;it!=end;++it)cerr<<*it;
    asio::async_read(*ais,*pbuf,std::bind(read_handler,_1,_2,ais,pbuf));
  }else{
    ais->cancel();
    ais->close();
  }
}
// close a file descriptor
int eclose(int fd,bool throwExcept=true){
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
// (returns child process pid)
int spawnPipeChild(string const&file,vector<string>args,int&fdRead,int&fdWrite,bool dieWhenParentDies){
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
  int stat=fork();
  if(stat==0){ // child
    // die if parent dies so we won't become a zombie
    if(dieWhenParentDies&&prctl(PR_SET_PDEATHSIG,SIGHUP)<0){
      string err{strerror(errno)};
      THROW_RUNTIME("spawnPipeChild: failed call to prctl(...): "<<err);
    }
    // dup stdin/stdout ---> pipe, and close original pipe fds
    eclose(0);eclose(1);
    edup(toChild[0]);edup(fromChild[1]);
    eclose(toChild[1]);eclose(fromChild[0]);
    eclose(toChild[0]);eclose(fromChild[1]);

    // setup for calling execv 
    // (we don't care if we leak memory since we'll overlay process using exec)
    char*const*tmpargs=(char*const*)malloc((args.size()+1)*sizeof(char*const*));
    if(tmpargs==0){
      string err{strerror(errno)};
      THROW_RUNTIME("spawnPipeChild: failed call to malloc(...): "<<err);
    }
    char**ptmpargs=const_cast<char**>(tmpargs);
    for(int i=0;i<args.size();++i)ptmpargs[i]=const_cast<char*>(args[i].c_str());
    ptmpargs[args.size()]=0;

    // execute cat program
    if(execv(file.c_str(),tmpargs)<0){
      string err{strerror(errno)};
      THROW_RUNTIME("spawnPipeChild: failed executing execl: "<<err);
    }else{
      // should never get here
      string err{strerror(errno)};
      THROW_RUNTIME("spawnPipeChild: failed executing execl: "<<err);
    }
  }else
  if(stat>0){ // parent
    // close fds we don't use
    eclose(fromChild[1]);eclose(toChild[0]);

    // set return parameters and return child pid
    fdRead=fromChild[0];
    fdWrite=toChild[1];
    return stat;
  }else{
    // fork failed
    string err{strerror(errno)};
    THROW_RUNTIME("spawnPipeChild: failed fork: "<<err);
  }
}
// main test program
int main(){
  try{
    // io_service object
    asio::io_service ios;

    // spawn child
    int fdRead;
    int fdWrite;
    string execFile{"/bin/cat"};
    vector<string>execArgs{"cat"};
    int cpid=spawnPipeChild(execFile,execArgs,fdRead,fdWrite,true);

    // write to child
    asio::posix::stream_descriptor aos(ios,fdWrite);
    asio::streambuf tmpbuf{1024};
    ostream os{&tmpbuf};
    os<<"Hello world"<<endl<<flush;
    asio::write(aos,tmpbuf);

    // read from child asynchronously
    asio::streambuf buf{1}; // NOTE! Should not have to be of size '1'
    asio::posix::stream_descriptor ais(ios,fdRead);
    asio::async_read(ais,buf,std::bind(read_handler,_1,_2,&ais,&buf));

    // setup deadline timer to close write fd to child after a few seconds
    boost::asio::deadline_timer ticker(ios,boost::posix_time::milliseconds(1000));
    std::function<void(const boost::system::error_code&)>fticker=[&](const boost::system::system_error&ec){
      cerr<<"ticker: closing write to child fd ..."<<endl;
      eclose(fdWrite,false);
    };
    ticker.async_wait(fticker);

    // run asio
    cerr<<"running asio ..."<<endl;
    ios.run();

    // wait for child
    cerr<<"waiting for child ..."<<endl;
    int waitstat;
    waitpid(-1,&waitstat,0);
  }
  catch(std::exception const&e){
    cerr<<"main: coufg exception: "<<e.what()<<endl;
  }
}

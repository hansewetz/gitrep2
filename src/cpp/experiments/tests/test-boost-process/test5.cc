/*
problem is that 'async_read(...)' reads until buffer is full.
right now, buffer size is set to 1.
should instead use 'async_read_until(...) or something else
*/
#include <string>
#include <vector>
#include <array>
#include <functional>
#include <iterator>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/asio.hpp>
using namespace std;
using namespace std::placeholders;
namespace asio=boost::asio;

// global buffer
asio::streambuf buf{1};

// callback handler for read
void read_handler(boost::system::error_code const&err,size_t nbytes,asio::posix::stream_descriptor*ais){
  if(nbytes!=0){
    istreambuf_iterator<char>end;
    istreambuf_iterator<char>it{&buf};
    for(;it!=end;++it)cerr<<*it;
    asio::async_read(*ais,buf,std::bind(read_handler,_1,_2,ais));
  }else{
    ais->cancel();
    ais->close();
  }
}
// set fd to non-blocking
void setnonblock(int fd){
  int flags = fcntl(fd,F_GETFL,0);
  fcntl(fd, F_SETFL,flags|O_NONBLOCK); 
}
// main test program
int main(){
  // io_service object
  asio::io_service ios;

  // create pipe between child and parent
  int fromChild[2];
  int toChild[2];
  if(pipe(toChild)!=0||pipe(fromChild)!=0){
    cerr<<"pipe failed"<<endl;
    exit(1);
  }
  // set parent read side to non-blocking
  setnonblock(fromChild[1]);

  // fork child process
  int stat=fork();
  if(stat==0){ // child
    // dup stdin/stdout ---> pipe
    close(0);close(1);
    dup(toChild[0]);dup(fromChild[1]);
    close(toChild[1]);close(fromChild[0]);
    close(toChild[0]);close(fromChild[1]);

    // execyte cat program
    if(execl("/bin/cat",'\0')<0){
      cerr<<"execl failed"<<endl;
      exit(1);
    }
    exit(0);
  }else
  if(stat>0){
    // close fds we don't use
    close(fromChild[1]);close(toChild[0]);

    // write to child
    asio::posix::stream_descriptor aos(ios,toChild[1]);
    asio::streambuf tmpbuf{1024};
    ostream os{&tmpbuf};
    os<<"Hello world"<<endl<<flush;
    asio::write(aos,tmpbuf);

    // read from other end of pipe asynchronously
    asio::posix::stream_descriptor ais(ios,fromChild[0]);
    asio::async_read(ais,buf,std::bind(read_handler,_1,_2,&ais));

    // setup deadline timer to close write fd to child after a few seconds
    boost::asio::deadline_timer ticker(ios,boost::posix_time::milliseconds(1000));
    std::function<void(const boost::system::error_code&)>fticker=[&](const boost::system::system_error&ec){
      cerr<<"ticker: closing write to child fd ..."<<endl;
      close(toChild[1]);
    };
    ticker.async_wait(fticker);

    // run asio
    cerr<<"running asio ..."<<endl;
    ios.run();

    // wait for child
    cerr<<"waiting for child ..."<<endl;
    int waitstat;
    waitpid(-1,&waitstat,0);
  }else{ // error
    cerr<<"fork error"<<endl;
    exit(1);
  }
}

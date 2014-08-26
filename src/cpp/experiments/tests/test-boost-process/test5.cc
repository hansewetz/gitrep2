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
asio::streambuf buf;

// callback handler for read
void read_handler(boost::system::error_code const&err,size_t nbytes,asio::posix::stream_descriptor*ais){
  if(nbytes!=0){
    istreambuf_iterator<char>end;
    istreambuf_iterator<char>it{&buf};
    for(;it!=end;++it)cout<<*it;
    asio::async_read(*ais,buf,std::bind(read_handler,_1,_2,ais));
  }else{
    ais->cancel();
    ais->close();
  }
}
// main test program
int main(){
  // io_service object
  asio::io_service ios;

  // create pipe between child and parent
  int p[2];
  if(pipe(p)!=0){
    cerr<<"pipe failed"<<endl;
    exit(1);
  }
  // fork child process
  int stat=fork();
  if(stat==0){ // child
    // close read end of pipe
    close(p[0]);

    // write some stuff to write end of pipe
    for(int i=0;i<10;++i){
      string str{"Hello\n"};
      write(p[1],str.data(),str.length());
    }
    exit(0);
  }else
  if(stat>0){
    // close write end of pipe
    close(p[1]);

    // read from other end of pipe asynchronously
    asio::posix::stream_descriptor ais(ios,p[0]);
    asio::async_read(ais,buf,std::bind(read_handler,_1,_2,&ais));

    // run asio
    ios.run();

    // wait for child
    int waitstat;
    waitpid(-1,&waitstat,0);
  }else{ // error
    cerr<<"fork error"<<endl;
    exit(1);
  }
}

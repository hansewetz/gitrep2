#include "utils/FdAsyncLineReader.h"
#include "utils/sysUtils.h"
#include <unistd.h>
#include <sys/wait.h>
#include <boost/asio.hpp>

using namespace std;
using namespace utils;

// main for test program
int main(){
  try{
    // io_service object
    boost::asio::io_service ios;

    // spawn /bin/cat as child program
    int fdRead,fdWrite;
    int cpid=spawnPipeChild("/bin/cat",vector<string>{"cat"},fdRead,fdWrite,true);

    // setup reading from child asynchronously and capture each read line in a callback function
    // (will invoke callback function with a string after stripping it form newline)
    function<void(string const&)>linecb=[](string const&line){cerr<<line<<endl;};
    function<void(boost::system::error_code const&err)>errcb=[](boost::system::error_code const&err){cerr<<err.message()<<endl;};
    FdAsyncLineReader fdr{ios,fdRead,3,linecb,errcb};

    // setup deadline timer to close write fd to child after a few seconds
    boost::asio::deadline_timer ticker(ios,boost::posix_time::milliseconds(1000));
    std::function<void(const boost::system::error_code&)>fticker=[&](const boost::system::system_error&ec){
      cerr<<"ticker: closing pipe to child fd ..."<<endl;
      eclose(fdWrite,false);
    };
    ticker.async_wait(fticker);

    // write to child
    constexpr char msg[]="Hello world 1\nAgain and again\nTesting translation again ...\n";
    write(fdWrite,msg,sizeof(msg));

    // run asio
    cerr<<"running asio ..."<<endl;
    ios.run();

    // wait for child
    cerr<<"waiting for child ..."<<endl;
    int waitstat;
    while(waitpid(-1,&waitstat,0)>0);
  }
  catch(std::exception const&e){
    cerr<<"main: cought exception: "<<e.what()<<endl;
  }
}

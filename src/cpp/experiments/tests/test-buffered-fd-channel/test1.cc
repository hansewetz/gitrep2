// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#ifndef __BUFFERD_FD_CHANNEL_HH__
// written to conform with C++/98 standard (not C++/0x11)
#include <iostream>
#include <exception>
#include <cstddef>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>

#include <boost/noncopyable.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/lexical_cast.hpp>

/**
 *  @TODO (1) error management is crap - must fix it in the read/write functions. Especially must manage return value of EAGAIN better
 *  @TODO (2) break up file into header/cc file
 *  @TODO (3) add signal handling
 */

/** simple template buffered channel class hack reading from one file decriptor and writing to another on.
 *
 *  the class can handle any type of integral data even though the most common one should be single byte data
 *  the class buffers data in a circular buffer which can be used to extend a pipe of limited size.
 *  the buffer is fixed an cannot be modified ones a channel has been constructed.
 *  this class does not setup or manage signals - this must be done from the client side.
 *
 *  @param scalar type to be read/written
 *
 */
template<typename T>
class BufferedFdChannel:boost::noncopyable{
public:
  /** constructor
   *
   *  this is the only constructor, all others are disabled.
   *
   *  fdin    fd to read from
   *  fdout   fd to write to
   *  maxsize maximum size of buffer. The buffer is fixed and will not grow
   *  dummy   dummy argument to prevent compilation with non-scalar template argument
   */
  BufferedFdChannel(int fdin,int fdout,std::size_t maxSize,size_t heartbeatSec,typename boost::enable_if<boost::is_integral<T>,T>::type*dummy=0):
      fdin_(fdin),fdout_(fdout),buf_(maxSize),heartbeatSec_(heartbeatSec){
    // make sure fds are in non-blocked mode
    setFdNonBlocking(fdin_);
    setFdNonBlocking(fdout_);
  }
  /**
   * start channel
   *
   * channel will exit when encountering EOF or in the case of a pipe, the reader/writer process(es) has exited
   */
  void run(){
    // loop until EOF (file EOF or pipe looses reader/writer process)
    while(true){
      // setup for what to listen on
      fd_set input;
      fd_set output;
      FD_ZERO(&input);
      FD_ZERO(&output);
      int maxfd=-1;
      if(!buf_.full()){
        FD_SET(fdin_,&input);
        maxfd=fdin_;
      }
      if(!buf_.empty()){
        FD_SET(fdout_,&output);
        maxfd=std::max(maxfd,fdout_);
      }
      // setup for heartbeat
      struct timeval tmo;
      if(heartbeatSec_>0){
        tmo.tv_sec=heartbeatSec_;
        tmo.tv_usec=0;
      }
      // block on select - timeout if configured
      assert(maxfd!=-1);
      int n=select(++maxfd,&input,&output,NULL,(heartbeatSec_>0)?&tmo:NULL);

      // check result of select
      if(n<0)throw std::runtime_error(std::string("select() failed, errno: ")+boost::lexical_cast<std::string>(errno));
      if(n==0){
        std::cerr<<"<HEARTBEAT>>"<<std::flush<<std::endl;
        continue;
      }
      // brute force logic checking even if we don't need to check
      // (make sure to break out if we get EOF (read/wrote 0 bytes when we should be able to read/write)
      // (notice that reading/writing 0 bytes from a pipe means that either the reading/writing process died)
      if(FD_ISSET(fdin_,&input)){
        int nread=readData();
        if(nread<=0&&buf_.size()==0)break;
      }
      if(FD_ISSET(fdout_,&output)){
        int nwritten=writeData();
        if(nwritten<=0&&buf_.size()==0)break;
      }
    }
  }
private:
  // helper method writing data from buffer - returns -errno or #bytes processed
  int writeData(){
    // check for empty buffer first (this should never happen since we controll it in run() method)
    // (we can linearize buffer and write directly from buffer and then pop all elements in one shot)
    if(buf_.empty())return 0;

    // write and then pop from queue (simplify by linearizing circular buffer ... OK, a little slower, but simpler)
    int ret=0;
    while((ret=write(fdout_,buf_.linearize(),sizeof(T)*buf_.size()))==-1&&errno==EINTR);
    int errnosave=errno;
    if(ret>0){
      typename boost::circular_buffer<T>::iterator itend=buf_.begin();
      std::advance(itend,ret);
      buf_.erase(buf_.begin(),itend);
    }
    // return total number of bytes written or 
    return ret>=0?ret/sizeof(T):-errnosave;
  }
  // helper method reading data into buffer - returns -errno or #bytes processed
  // (there is no good way of reading directly into buffer and then updating buffer state)
  // (for that I would have to write my own buffer and I'm to lazy to do that)
  int readData(){
    // check for full buffer first (this should never happen since we controll it in run() method)
    if(buf_.full())return 0;

    // read one item at a time (slow ... but to tired to be clever)
    int max2read=buf_.capacity()-buf_.size();
    int ret=0;
    int errnosave=0;
    while(true){
      T item;
      int nread=0;
      while(((nread=read(fdin_,&item,1))==-1&&errno==EINTR));
      errnosave=errno;
      if(nread<=0)break;
      buf_.push_back(item);
      ret+=nread;
      if(ret==max2read)break;
    }
    // return total number of bytes read
    return ret>=0?ret/sizeof(T):-errnosave;
  }
  // set fd to non-blocking mode
  void setFdNonBlocking(int fd){
    int flags=fcntl(fd,F_GETFL);
    if(flags<0)throw std::runtime_error(std::string("fcntl: could not get fd flags, errno: ")+boost::lexical_cast<std::string>(errno));
    int ret=fcntl(fd,F_SETFL,flags|O_NONBLOCK);
    if(ret<0)throw std::runtime_error(std::string("fcntl: could not set fd to non-blocking state, errno: ")+boost::lexical_cast<std::string>(errno));
  }
  // state of channel
  // (circular buffer + read/write file descriptors)
  int fdin_;
  int fdout_;
  boost::circular_buffer<T>buf_;
  std::size_t heartbeatSec_;
};
// typedef for most likely type of channel we'll use
typedef BufferedFdChannel<unsigned char>BufferedCharFdChannel;
#endif

// -----------------------------------------

#include <boost/program_options.hpp>
using namespace std;
namespace po=boost::program_options;

/**
 * main test program
 */
namespace{
po::options_description desc(string("usage: -s size -h"));
int bufferSize=4096;
int heartbeatSec=0;
void usage(){
  std::cerr<<desc;
  std::exit(1);
}
}
int main(int argc,char*argv[]){
  try{
    // process command line parameters
    desc.add_options()
      ("help,h","print help message")
      ("size,s",po::value<int>(),"buffer size in bytes - must be greater than zero")
      ("beat,b",po::value<int>(),"heartbeat- must be greater than zero");
    po::variables_map vm;
    po::store(po::parse_command_line(argc,argv,desc),vm);
    po::notify(vm); 

    // check for help
    if(vm.count("help")>0){
      usage();
    }
    // check for buffer size
    if(vm.count("size")){
      if((bufferSize=vm["size"].as<int>())<=0)usage();
    }
    // check for heartbeat
    if(vm.count("beat")>0){
      if((heartbeatSec=vm["beat"].as<int>())<=0)usage();
    }
    // create channel and start it
    BufferedCharFdChannel ch(0,1,bufferSize,heartbeatSec);
    ch.run();
  }
  catch(std::exception&e){
    std::cerr<<"error: caught exception: \""<<e.what()<<"\""<<std::endl;
  }
}

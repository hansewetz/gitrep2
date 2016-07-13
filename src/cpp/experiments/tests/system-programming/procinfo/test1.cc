// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include "boost/filesystem.hpp"
#include <boost/regex.hpp>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>
using namespace std;
namespace fs=boost::filesystem;

// struct used when reading line in proc/<pid>/stat file
struct procstat_t{
  string name;
  int pid;
  int ppid;
  int pgid;
  int sessid;
  unsigned long vmsize;           // virtual memory size in bytes
  unsigned long utime_sec;        // time in user mode (sec)
  long rss;                       // amount of memory in bytes allocated to a process and is in RAM
};
ostream&operator<<(ostream&os,procstat_t const&p){
  return os<<p.pid<<", ppid: "<<p.ppid<<", pgid: "<<p.pgid<<", vmsize: "<<p.vmsize<<", utime_sec: "<<p.utime_sec<<", rss: "<<p.rss<<" --> "<< p.name;
}
// given a PGID get info about all processes within the group
vector<procstat_t>getpgidstat(int pgid){
  vector<procstat_t>ret;

  // get all process ids on the machine by reading the process directory
  vector<int>pids;
  fs::directory_iterator itend;
  for(fs::directory_iterator it("/proc/");it!=itend;++it){
    if(is_directory(it->path())){
      // get leaf - if it is a positve integer then save it
      string leaf=it->path().filename().string();
      static const boost::regex re{("\\d+")};
      if(!boost::regex_match(leaf,re))continue;
      pids.push_back(stoi(leaf));
    }
  }
  // get info for all processes belonging to progress group
  int tps=sysconf(_SC_CLK_TCK);   
  long  pagesize=sysconf(_SC_PAGE_SIZE);
  for(auto const&p:pids){
    // open file for read
    char statfile[256];
    sprintf(statfile, "/proc/%d/stat",p);
    int fd=open(statfile,O_RDONLY);
    if(fd<0)continue;

    // read a line from file and close it
    char data[2048];
    int readstat=read(fd,data,2048);
    close(fd);
    if(readstat<0)continue;

    // get data from read data
    procstat_t procstat;
    char name[1024];
    sscanf(data, "%d %s %*c %d %d %d %*d %*d %*u %*u %*u %*u %*u %lu "
                 "%*u %*d %*d %*d %*d %*d %*d %*u %lu %ld",
                 &procstat.pid,name,&procstat.ppid,&procstat.pgid,&procstat.sessid,&procstat.utime_sec,&procstat.vmsize,&procstat.rss);

    // skip if not same pid and adjust some values
    if(procstat.pgid!=pgid)continue;
    procstat.name=name;
    procstat.utime_sec/=tps;
    procstat.rss*=pagesize;
    ret.push_back(procstat);
  }
  return ret;
}
// main test program
int main(int argc,char*argv[]){
  int pgid;
  sscanf(argv[1],"%d",&pgid);
  auto pstats=getpgidstat(pgid);
  for(auto const&p:pstats)cout<<p<<endl;
}

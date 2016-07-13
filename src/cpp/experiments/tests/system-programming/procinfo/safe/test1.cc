// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include "boost/lexical_cast.hpp"
#include "boost/filesystem.hpp"
#include <boost/regex.hpp>
#include <sstream>
#include <iterator>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;
namespace fs=boost::filesystem;

// split a string on a blank
vector<string>splitStringOnBlanks(string const&str){
  istringstream iss(str);
  return vector<string>(istream_iterator<string>(iss),istream_iterator<string>());
}
// struct used when reading line in proc/<pid>/stat file
struct procstat_t{
  int pid;
  int ppid;
  int pgid;
  int sessid;
  unsigned long vmsize;
};
ostream&operator<<(ostream&os,procstat_t const&p){
  return os<<"pid: "<<p.pid<<", ppid: "<<p.ppid<<", pgid: "<<p.pgid<<", vmsize: "<<p.vmsize;
}
// given a PID, build a tree with the PID as root
// (tree is returned as a map where the key is a PID and the value is a list of children of the PID)
vector<procstat_t>getpgidstat(int pgid){
  vector<procstat_t>ret;

  // root directory
  fs::path procdir{"/proc"};

  // get all process ids on the machine by reading the process directory
  vector<string>piddirs;
  fs::directory_iterator itend;
  for(fs::directory_iterator it(procdir);it!=itend;++it){
    if(is_directory(it->path())){
      // get leaf - if it is a positve integer then save it
      string leaf=it->path().filename().string();
      static const boost::regex re{("\\d+")};
      if(!boost::regex_match(leaf,re))continue;
      piddirs.push_back(it->path().string());
    }
  }
  // get info for all processes belonging to progress group
  for(auto const&piddir:piddirs){
    // open stat file for this pid
    // (we might fail if process has terminated)
    string statfile=piddir+"/stat";
    ifstream is(statfile.c_str(),std::ifstream::in);
    if(!is)continue;
    string line;
    getline(is,line);
    if(!is)continue;
    auto tokens=splitStringOnBlanks(line);

    // retrieve the info we need - skip if not same pgid
    procstat_t procstat;
    procstat.pgid=stoi(tokens[4]);
    if(procstat.pgid!=pgid)continue;
    procstat.pid=stoi(tokens[0]);
    procstat.ppid=stoi(tokens[3]);
    procstat.sessid=stoi(tokens[4]);
    procstat.vmsize=stol(tokens[22]);
    ret.push_back(procstat);
  }
  return ret;
}
// main test program
int main(){
  int pgid;
  cin>>pgid;
  auto pstats=getpgidstat(pgid);
  for(auto const&p:pstats)cout<<p<<endl;
}

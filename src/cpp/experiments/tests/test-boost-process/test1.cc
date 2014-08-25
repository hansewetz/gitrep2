#include <string>
#include <vector>
#include <iostream>
#include <boost/process.hpp>
#include <boost/filesystem.hpp>
using namespace std;
namespace bp=boost::process;
namespace fs=boost::filesystem;

// main test program
int main(){
  fs::path prg{"/bin/ls"};
  bp::child c{bp::execute(bp::initializers::run_exe(prg))};
  bp::wait_for_exit(c);
}

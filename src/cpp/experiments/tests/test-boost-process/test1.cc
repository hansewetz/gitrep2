#include <string>
#include <vector>
#include <iostream>
#include <boost/process.hpp>
using namespace std;
namespace bp=boost::process;

// main test program
int main(){
  vector<string>prog{"/bin/ls","-l"};
  bp::child c{bp::execute(bp::initializers::set_args(prog),
              bp::initializers::start_in_dir("."),
              bp::initializers::throw_on_error())};
  bp::wait_for_exit(c);
}

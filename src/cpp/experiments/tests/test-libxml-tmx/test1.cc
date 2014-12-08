#include "NorwegianTmxExtractor.h"
#include <iostream>
using namespace std;
using namespace mtdata;

// main test program
int main(int argc, char **argv) {
  if (argc!=2){
    fprintf(stderr,"usage: %s <xml-file>\n",argv[0]);
    return(1);
  }
  // create NorwegianTmxExtractor object
  NorwegianTmxExtractor norExtract{argv[1]};

  // print some data
  cout<<"dbid: \""<<norExtract.dbid()<<"\""<<endl;
}

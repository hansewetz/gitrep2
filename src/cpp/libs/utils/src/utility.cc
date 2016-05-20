// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include "utils/utility.h"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
using namespace std;
namespace utils{

// generate a uuid
string generateUuid(){
  return boost::lexical_cast<string>(boost::uuids::random_generator()());
}
}

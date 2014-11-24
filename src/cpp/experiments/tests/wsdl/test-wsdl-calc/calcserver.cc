// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "soapcalcService.h"
#include "calc.nsmap"
#include <boost/lexical_cast.hpp>
#include <iostream>
using namespace std;

// --- server
int main(int argc, char **argv){
  if(argc!=2){
    cerr<<"usage: "<<argv[0]<<" <port>"<<endl;
    exit(-1);
  }
  // get calculation service
  calcService calc;

  // get port
  int port = boost::lexical_cast<int>(argv[1]);
  if (!port){
    calc.soap_stream_fault(std::cerr);
    exit(-1);
  }
  // run service (listen for client connections)
  if (calc.run(port)){
   calc.soap_stream_fault(std::cerr);
    exit(1);
  }
  return 0;
} 

//  --- methods
int calcService::add(double a, double b, double *result)
{ *result = a + b;
  return SOAP_OK;
} 

int calcService::sub(double a, double b, double *result)
{ *result = a - b;
  return SOAP_OK;
} 

int calcService::mul(double a, double b, double *result)
{ *result = a * b;
  return SOAP_OK;
} 

int calcService::div(double a, double b, double *result)
{ if (b)
    *result = a / b;
  else
  { char *s = (char*)soap_malloc(this, 1024);
    sprintf(s, "<error xmlns=\"http://tempuri.org/\">Can't divide %f by %f</error>", a, b);
    return soap_senderfault("Division by zero", s);
  }
  return SOAP_OK;
} 

int calcService::pow(double a, double b, double *result)
{ *result = ::pow(a, b);
  if (soap_errno == EDOM)	/* soap_errno is like errno, but compatible with Win32 */
  { char *s = (char*)soap_malloc(this, 1024);
    sprintf(s, "Can't take the power of %f to %f", a, b);
    sprintf(s, "<error xmlns=\"http://tempuri.org/\">Can't take power of %f to %f</error>", a, b);
    return soap_senderfault("Power function domain error", s);
  }
  return SOAP_OK;
}

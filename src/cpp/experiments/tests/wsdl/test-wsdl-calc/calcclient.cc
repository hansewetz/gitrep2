// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include "soapcalcProxy.h"
#include "calc.nsmap"

//const char server[] = "http://websrv.cs.fsu.edu/~engelen/calcserver.cgi";
const char server[] = "http://localhost:6060";

int main(int argc, char **argv){
   if (argc < 4){
     fprintf(stderr, "Usage: [add|sub|mul|div|pow] num num\n");
    exit(0);
  }
  double a, b, result;
  a = strtod(argv[2], NULL);
  b = strtod(argv[3], NULL);
  calcProxy calc;
  calc.soap_endpoint = server;
  switch (*argv[1]){
    case 'a':
      calc.add(a, b, &result);
      break;
    case 's':
      calc.sub(a, b, &result);
      break;
    case 'm':
      calc.mul(a, b, &result);
      break;
    case 'd':
      calc.div(a, b, &result);
      break;
    case 'p':
      calc.pow(a, b, &result);
      break;
    default:
      fprintf(stderr, "Unknown command\n");
      exit(0);
  }
  if (calc.error)
    calc.soap_stream_fault(std::cerr);
  else
    printf("result = %g\n", result);
  return 0;
}


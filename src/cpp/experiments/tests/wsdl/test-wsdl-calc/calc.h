// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

//gsoap ns service name:	calc Simple calculator service
//gsoap ns service protocol:	SOAP
//gsoap ns service style:	rpc
//gsoap ns service encoding:	encoded
//gsoap ns service namespace:	http://websrv.cs.fsu.edu/~engelen/calc.wsdl
//gsoap ns service location:	http://websrv.cs.fsu.edu/~engelen/calcserver.cgi

//gsoap ns schema namespace:	urn:calc

//gsoap ns service method-documentation: add Sums two values
int ns__add(double a, double b, double *result);

//gsoap ns service method-documentation: sub Subtracts two values
int ns__sub(double a, double b, double *result);

//gsoap ns service method-documentation: mul Multiplies two values
int ns__mul(double a, double b, double *result);

//gsoap ns service method-documentation: div Divides two values
int ns__div(double a, double b, double *result);

//gsoap ns service method-documentation: pow Raises a to b
int ns__pow(double a, double b, double *result);

// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

//gsoap ns service name:	xlate Simple xlator service
//gsoap ns service protocol:	SOAP
//gsoap ns service style:	rpc
//gsoap ns service encoding:	encoded
//gsoap ns service namespace:	http://maffei.cc.cec.eu.ent/~ewetzha/xlate.wsdl
//gsoap ns service location:	http:///maffei.cc.cec.eu.ent/~ewetzha/xlateserver.cgi

//gsoap ns schema namespace:	urn:xlate

int ns__translate(double a,struct ns__asString{char*str;}&r);


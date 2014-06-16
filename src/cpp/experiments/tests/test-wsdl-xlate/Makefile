# a bunch of files generated form header file

CAL_DEP= \
	xlate.convert.req.xml xlate.convert.res.xml xlate.nsmap xlate.wsdl ns.xsd soapC.cpp soapH.h \
	soapStub.h soapxlateProxy.cpp soapxlateProxy.h soapxlateService.cpp soapxlateService.h 

# all targets:
all: xclient xserver

# client
xclient: xlate.h xclient.cc stdsoap2.cpp soapC.cpp soapxlateProxy.cpp ${CALC_DEP}
	g++ ${STDFLAG} -o xclient xclient.cc stdsoap2.cpp soapC.cpp soapxlateProxy.cpp -I${BOOST_INC} -L${BOOST_LIB} -lboost_program_options

# server
xserver: xlate.h xserver.cc stdsoap2.cpp soapC.cpp soapxlateProxy.cpp ${CALC_DEP}
	g++ ${STDFLAG} -o xserver xserver.cc stdsoap2.cpp soapC.cpp soapxlateService.cpp -I${BOOST_INC} -L${BOOST_LIB} -lboost_program_options

# generate stubs, skeletons, proxies and marchaling code
${CAL_DEP}: xlate.h
	soapcpp2 -i xlate.h
	
# clean up
clean:
	rm -f soap* xserver xclient *.xml *nsmap *.xsd *.o *.wsdl



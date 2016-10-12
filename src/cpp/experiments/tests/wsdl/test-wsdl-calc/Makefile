# a bunch of files generated form header file
CAL_DEP= \
	calc.add.req.xml calc.add.res.xml calc.div.req.xml calc.div.res.xml calc.mul.req.xml calc.mul.res.xml calc.nsmap \
	calc.pow.req.xml calc.pow.res.xml calc.sub.req.xml calc.sub.res.xml calc.wsdl ns.xsd \
	soapC.cpp soapH.h soapStub.h soapcalcProxy.cpp soapcalcProxy.h soapcalcService.cpp soapcalcService.h

# all targets:
all: calcclient calcserver

# client
calcclient: calc.h calcclient.cc stdsoap2.cpp soapC.cpp soapcalcProxy.cpp ${CALC_DEP}
	g++ ${STDFLAG} -o calcclient calcclient.cc stdsoap2.cpp soapC.cpp soapcalcProxy.cpp

# server
calcserver: calc.h calcserver.cc stdsoap2.cpp soapC.cpp soapcalcProxy.cpp ${CALC_DEP}
	g++ ${STDFLAG} -o calcserver calcserver.cc stdsoap2.cpp soapC.cpp soapcalcService.cpp -I${BOOST_INC}

# generate stubs, skeletons, proxies and marchaling code
${CAL_DEP}: calc.h
	soapcpp2 -i calc.h
	
# clean up
clean:
	rm -f soap* calcserver calcclient *.xml *nsmap *.xsd *.o *.wsdl

# run server
run: all
	./calcserver 6060&

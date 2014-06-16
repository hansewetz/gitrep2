# a bunch of files generated form header file
CAL_DEP= \
	quote.h soapC.c soapH.h soapStub.h StockQuoteSoapBinding.GetLastTradePrice.req.xml StockQuoteSoapBinding.GetLastTradePrice.res.xml StockQuoteSoapBinding.nsmap \
	soapClient.c soapClientLib.c  

# all targets:
all: client 

# client
client: quote.h client.cc stdsoap2.cpp soapC.c soapClient.c soapClientLib.c  ${CALC_DEP}
	g++ ${STDFLAG} -o client client.cc stdsoap2.cpp soapC.c soapClient.c -I${BOOST_INC} -L${BOOST_LIB} -lboost_program_options

# generate stubs, skeletons, proxies and marchaling code
${CAL_DEP}: quote.wsdl
	wsdl2h -c -o quote.h quote.wsdl
	soapcpp2 -I${INSTALL_EXTRA_HOME}/share/gsoap/import quote.h

# generate headerfile from wsdl
quote.h: quote.wsdl
	wsdl2h quote.wsdl
	
# clean up
clean:
	rm -f client ${CAL_DEP} soapServer.*  soapServerLib.*


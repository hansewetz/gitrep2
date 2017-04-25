# a bunch of files generated form header file
CALC_DEP= \
	quote.h soapC.c soapH.h soapStub.h StockQuoteSoapBinding.GetLastTradePrice.req.xml StockQuoteSoapBinding.GetLastTradePrice.res.xml StockQuoteSoapBinding.nsmap \
	soapClient.c soapClientLib.c  

# all targets:
all: client 

# client
client: quote.h client.cc stdsoap2.cpp soapC.c soapClient.c soapClientLib.c  ${CALC_DEP}
	g++  -m64 -std=c++17 -Wno-deprecated -Wno-register -D_GLIBCXX_USE_CXX11_ABI=0 -o client client.cc stdsoap2.cpp soapC.c soapClient.c -I${BOOST_INC} -L${BOOST_LIB} -lboost_program_options

# generate stubs, skeletons, proxies and marchaling code
${CALC_DEP}: quote.wsdl
	wsdl2h -c -o quote.h quote.wsdl
	soapcpp2 -I${INSTALL_EXTRA_HOME}/share/gsoap/import quote.h

# generate headerfile from wsdl
quote.h: quote.wsdl
	wsdl2h quote.wsdl
	
# clean up
clean:
	rm -f client ${CALC_DEP} soapServer.*  soapServerLib.*


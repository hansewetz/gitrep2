test1: test1.cxx hello.xsd
	xsd cxx-tree hello.xsd
	g++ ${STDFLAG}  -I$(INSTALL_HOME)/include -I$(INSTALL_EXTRA_HOME)/include -I$(INSTALL_EXTRA_HOME)/include/libxsd -L$(INSTALL_HOME)/lib -L$(INSTALL_EXTRA_HOME)/lib -lxerces-c -o test1 test1.cxx hello.cxx

clean:
	rm -f hello.o hello.?xx test1.o test1


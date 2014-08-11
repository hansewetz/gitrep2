test1: test1.cc
	g++ ${STDFLAG} -o test1 test1.cc -I$(BOOST_INC) -I$(INSTALL_EXTRA_HOME)/include -I$(INSTALL_EXTRA_HOME)/iclude/Wt -L$(BOOST_LIB) -L$(INSTALL_EXTRA_HOME)/lib -L/usr/lib -lwt -lwthttp -lboost_signals

run:
	./test1 --docroot . --http-address 0.0.0.0 --http-port 8080

clean:
	rm -f test1 *.o

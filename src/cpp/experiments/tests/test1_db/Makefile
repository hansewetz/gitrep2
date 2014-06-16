all: test_write test_read

test_write: test_write.cc
	g++ ${STDFLAG} -DHAVE_CXX_STDHEADERS -I$(INSTALL_HOME)/include -L$(INSTALL_HOME)/lib -I$(INSTALL_EXTRA_HOME)/include -L$(INSTALL_EXTRA_HOME)/lib -ldb_cxx -o test_write test_write.cc

test_read: test_read.cc
	g++ ${STDFLAG} -DHAVE_CXX_STDHEADERS -I$(INSTALL_HOME)/include -L$(INSTALL_HOME)/lib -I$(INSTALL_EXTRA_HOME)/include -L$(INSTALL_EXTRA_HOME)/lib -ldb_cxx -o test_read test_read.cc

clean:
	rm -f test_write test_read *.o test.db

write: test_write
	./test_write

read: test_read
	./test_read

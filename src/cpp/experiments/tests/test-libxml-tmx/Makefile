# To be built
LOCAL_TARGET    =  test1
LOCAL_SOTARGET  =
LOCAL_OBJS      = test1.o NorwegianTmxExtractor.o

# Control.
LOCAL_LIBS      = -lxml2 -lboost_filesystem -lboost_log -lboost_system -lpthread
LOCAL_LIBPATH   = -L$(INSTALL_EXTRA_HOME)/lib -L$(BOOST_INC)
LOCAL_INCPATH   = -I$(INSTALL_EXTRA_HOME)/include/libxml2  -I$(BOOST_INC)
LOCAL_DEFINES   = -DBOOST_ALL_DYN_LINK -DBOOST_IOSTREAMS_NO_LIB
LOCAL_CXXFLAGS  =
LOCAL_CPPFLAGS  = 
LOCAL_LDFLAGS   =
LOCAL_LDSOFLAGS =

include $(ENV_ROOT)/makerules/build.rules

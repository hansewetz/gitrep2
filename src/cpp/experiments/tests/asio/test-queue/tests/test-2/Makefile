# To be built
LOCAL_TARGET    =  test2
LOCAL_SOTARGET  =
LOCAL_OBJS      = test2.o

# Control.
LOCAL_LIBS      = -lboost_log -lboost_system -lboost_filesystem -lpthread -lboost_iostreams
LOCAL_LIBPATH   = -L$(PROJECT_ROOT)/lib/lib -L$(BOOST_LIB)
LOCAL_INCPATH   = -I${BOOST_INC} -I../..
LOCAL_DEFINES   = -DBOOST_ALL_DYN_LINK -DASIO_STANDALONE
LOCAL_CXXFLAGS  =
LOCAL_CPPFLAGS  = 
LOCAL_LDFLAGS   =
LOCAL_LDSOFLAGS =

include $(ENV_ROOT)/makerules/build.rules

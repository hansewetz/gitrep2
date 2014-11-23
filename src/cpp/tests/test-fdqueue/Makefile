# To be built
LOCAL_TARGET    =  test1
LOCAL_SOTARGET  =
LOCAL_OBJS      = test1.o

# Control.
LOCAL_LIBS      =  -lutils -lboost_system -lboost_log -lboost_filesystem -lboost_iostreams -lasioutils -lpthread

LOCAL_LIBPATH   = -L$(PROJECT_ROOT)/lib
LOCAL_INCPATH   = -I$(PROJECT_ROOT)/src/cpp/include/
LOCAL_DEFINES   = -DBOOST_ALL_DYN_LINK -DBOOST_IOSTREAMS_NO_LIB -DASIO_STANDALONE
LOCAL_CXXFLAGS  =
LOCAL_CPPFLAGS  = 
LOCAL_LDFLAGS   =
LOCAL_LDSOFLAGS =

include $(ENV_ROOT)/makerules/build.rules

# To be built
LOCAL_TARGET    =  test-dummy
LOCAL_SOTARGET  =
LOCAL_OBJS      = test-dummy.o

# Control.
LOCAL_LIBS      = -lbpfqueue -lbpfutils \
		  -lpthread \
		  -lboost_system -lboost_log -lboost_serialization -lboost_iostreams -lboost_thread -lboost_filesystem
LOCAL_LIBPATH   = -L$(PROJECT_ROOT)/lib
LOCAL_INCPATH   = -I$(PROJECT_ROOT)/src/cpp/include/
LOCAL_DEFINES   = -DBOOST_ALL_DYN_LINK -DBOOST_IOSTREAMS_NO_LIB
LOCAL_CXXFLAGS  =
LOCAL_CPPFLAGS  = 
LOCAL_LDFLAGS   =
LOCAL_LDSOFLAGS =

include $(ENV_ROOT)/makerules/build.rules

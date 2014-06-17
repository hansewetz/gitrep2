# To be built
LOCAL_TARGET    =  test-xlate-request
LOCAL_SOTARGET  =
LOCAL_OBJS      = test-xlate-request.o

# Control.
LOCAL_LIBS      = -lxlate-jobs -lutils \
		  -lboost_filesystem -lboost_iostreams
LOCAL_LIBPATH   = -L$(PROJECT_ROOT)/lib
LOCAL_INCPATH   = -I$(PROJECT_ROOT)/src/cpp/include/
LOCAL_DEFINES   = -DBOOST_ALL_DYN_LINK -DBOOST_IOSTREAMS_NO_LIB
LOCAL_CXXFLAGS  =
LOCAL_CPPFLAGS  = 
LOCAL_LDFLAGS   =
LOCAL_LDSOFLAGS =

include $(ENV_ROOT)/makerules/build.rules

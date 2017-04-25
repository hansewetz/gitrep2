# To be built
LOCAL_TARGET    =  test1 
LOCAL_SOTARGET  =
LOCAL_OBJS      = test1.o WtInputHandler.o WtLanSelectionWidget.o

# Control.
LOCAL_LIBS      = -lboost_iostreams -lwt -lwthttp -lboost_signals
LOCAL_LIBPATH   = -L$(BOOST_LIB) 
LOCAL_INCPATH   = -I$(BOOST_INC)
LOCAL_DEFINES   = -DBOOST_ALL_DYN_LINK -DBOOST_IOSTREAMS_NO_LIB -DBOOST_SIGNALS_NO_DEPRECATION_WARNING
LOCAL_CXXFLAGS  = -Wno-placement-new
LOCAL_CPPFLAGS  = 
LOCAL_LDFLAGS   =
LOCAL_LDSOFLAGS =

include $(ENV_ROOT)/makerules/build.rules

run:
	./test1 --docroot . --http-address 0.0.0.0 --http-port 8080

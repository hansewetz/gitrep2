# To be built
LOCAL_TARGET    =
LOCAL_SOTARGET  = $(PROJECT_ROOT)/lib/libbpfqueue.so
LOCAL_OBJS      = FdQueue.o InMemoryQueue.o InMemoryQueueRepository.o Message.o PolldirQueue.o Queue.o QueueRepository.o Utils.o

# Control.
LOCAL_LIBS      = 
LOCAL_LIBPATH   = 
LOCAL_INCPATH   = -I$(PROJECT_ROOT)/src/cpp/include -I$(BOOST_INC)

LOCAL_DEFINES   = -DBOOST_ALL_DYN_LINK -DBOOST_IOSTREAMS_NO_LIB
LOCAL_CXXFLAGS  = -Wno-expansion-to-defined -Wno-address
LOCAL_CPPFLAGS  =
LOCAL_LDFLAGS   =
LOCAL_LDSOFLAGS =

include $(ENV_ROOT)/makerules/build.rules

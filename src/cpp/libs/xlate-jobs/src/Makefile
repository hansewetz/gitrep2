# To be built
LOCAL_TARGET    =
LOCAL_SOTARGET  = $(PROJECT_ROOT)/lib/libxlate-jobs.so
LOCAL_OBJS      = TranslateRequest.o TranslationJob.o LanguageCode.o TranslationTask.o TaskQueue.o TranslationJobRepository.o TaskCollector.o TaskScheduler.o DummyTranslationEngine.o

# Control.
LOCAL_LIBS      = 
LOCAL_LIBPATH   = 
LOCAL_INCPATH   = -I$(PROJECT_ROOT)/src/cpp/include -I$(BOOST_INC)

LOCAL_DEFINES   = -DBOOST_ALL_DYN_LINK -DBOOST_IOSTREAMS_NO_LIB
LOCAL_CXXFLAGS  =
LOCAL_CPPFLAGS  =
LOCAL_LDFLAGS   =
LOCAL_LDSOFLAGS =

include $(ENV_ROOT)/makerules/build.rules

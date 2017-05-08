# setup some paths for where to find stuff
# (used in some makefiles)
if [[ ${DEV_ENV} == 'WORK' ]]; then
  # option for gcc compiler (see: makerules/build.rules)
  # (this is setup so that we can compile against gcc-=version 7)
  export STDFLAG="-std=c++17"                                      # must be c++14 or c+17
  export GCC_ERROR_IGNORE=""
  export GCC_USE_GLIBCXX11_ABI=""
  export GCC_ERROR_IGNORE="-Wno-deprecated -Wno-register"        # we get some warnings (which are turned into errors) with c++17
  export GCC_USE_GLIBCXX11_ABI="-D_GLIBCXX_USE_CXX11_ABI=0"      # gcc5+ uses a different ABI - we must specify this to avoid link errors
  
  # Shared drive containing C++ tool chain
  export SHAREDDRIVE=/ec/dgt/shared/exodus

# NOTE!  export INSTALL_HOME=/ec/prod/exodus/dgt/local/exodus/user/ewetzha/installs	# extra packages
  export INSTALL_HOME0=${SHAREDDRIVE}/local/CppEnv-V2.0
  export INSTALL_HOME=${SHAREDDRIVE}/local/CppEnv-V1.0
  export INSTALL_EXTRA_HOME=${SHAREDDRIVE}/local/CppEnv-V1.0					# basic install stuff
  export INSTALL_TEST_HOME='/ec/prod/exodus/dgt/local/exodus/user/ewetzha/installs-test'	# extra packages

  # oracle stuff
  export ORACLE_HOME=/ec/sw/oracle/client/product/11.2.0.2/

  # quantlib stuff
  export QLIB_PATH=/ec/prod/exodus/dgt/local/exodus/user/ewetzha/installs-test/quantlib
else
  # Compilation stuff
  export STDFLAG='-std=c++14'

  export INSTALL_HOME0=/home/hans/installs
  export INSTALL_HOME=/home/hans/installs
  export INSTALL_EXTRA_HOME=${INSTALL_HOME}
  export INSTALL_TEST_HOME=${INSTALL_HOME}
fi

# build environment root
export ENV_ROOT=`pwd`

# this variable is for backwards compatibility in Makefiles
export PROJECT_ROOT=${ENV_ROOT}/..

# must include boost stuff
if [[ ${DEV_ENV} == 'WORK' ]]; then
	# boost stuff
	export BOOST_INC=${INSTALL_HOME}/include
	export BOOST_LIB=${INSTALL_HOME}/lib

	# occi stuff
	export OCCI_INC=${INSTALL_EXTRA_HOME}/include/occi
	export OCCI_LIB=${INSTALL_EXTRA_HOME}/lib/occi

	# python stuff
	export PYTHONBIN3=/ec/prod/exodus/dgt/local/exodus/user/ewetzha/installs-python/bin
	export PYTHONBIN2=/ec/dgt/shared/exodus/local/python/bin
else
	# boost stuff
	export BOOST_INC=$INSTALL_HOME/include
	export BOOST_LIB=${INSTALL_HOME}/lib

	# occi stuff
	export OCCI_INC=${INSTALL_EXTRA_HOME}/include/occi
	export OCCI_LIB=${INSTALL_EXTRA_HOME}/lib/occi

	# python stuff
	export PYTHONBIN3=NOTE!
	export PYTHONBIN2=NOTE!
fi

# amend ld library path
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${PROJECT_ROOT}/lib/lib:${PROJECT_ROOT}/lib/test

# amend exe library path
export PATH=${PYTHONBIN3}:${PYTHONBIN2}:${INSTALL_HOME0}/bin:${INSTALL_HOME}/bin:${INSTALL_EXTRA_HOME}/bin:${PATH}:${PROJECT_ROOT}/bin/bin:${PROJECT_ROOT}/bin/test
export LD_LIBRARY_PATH=${INSTALL_HOME0}/lib64:${INSTALL_HOME}/lib:${INSTALL_HOME}/lib64:${INSTALL_HOME}/lib:${INSTALL_HOME}/lib64:${INSTALL_EXTRA_HOME}/lib:${LD_LIBRARY_PATH}:${PROJECT_ROOT}/lib

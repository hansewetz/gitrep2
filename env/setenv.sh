# Compilation stuff
export STDFLAG='-std=c++14'

# setup some paths for where to find stuff
# (used in some makefiles)
if [[ ${DEV_ENV} == 'WORK' ]]; then
  # Shared drive containing C++ tool chain
  export SHAREDDRIVE=/ec/dgt/shared/exodus

  export INSTALL_HOME=${SHAREDDRIVE}/local/CppEnv-V1.0					# basic install stuff
  export INSTALL_EXTRA_HOME=/ec/prod/exodus/dgt/local/exodus/user/ewetzha/installs	# extra packages
  export INSTALL_TEST_HOME='/ec/prod/exodus/dgt/local/exodus/user/ewetzha/installs-test'	# extra packages

  # oracle stuff
  export ORACLE_HOME=/ec/sw/oracle/client/product/11.2.0.2/

  # quantlib stuff
  export QLIB_PATH=/ec/prod/exodus/dgt/local/exodus/user/ewetzha/installs-test/quantlib
else
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
	export BOOST_INC=${INSTALL_HOME}/include
	export BOOST_LIB=${INSTALL_HOME}/lib
else
	export BOOST_INC=$INSTALL_HOME/include
	export BOOST_LIB=${INSTALL_HOME}/lib
fi
# occi stuff
export OCCI_INC=${INSTALL_HOME}/include/occi
export OCCI_LIB=${INSTALL_HOME}/lib/occi

# amend ld library path
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${PROJECT_ROOT}/lib/lib:${PROJECT_ROOT}/lib/test

# amend exe library path
export PATH=${INSTALL_HOME}/bin:${INSTALL_EXTRA_HOME}/bin:${PATH}:${PROJECT_ROOT}/bin/bin:${PROJECT_ROOT}/bin/test
export LD_LIBRARY_PATH=${INSTALL_HOME}/lib:${INSTALL_HOME}/lib64:${INSTALL_EXTRA_HOME}/lib:${LD_LIBRARY_PATH}:${PROJECT_ROOT}/lib

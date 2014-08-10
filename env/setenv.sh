# ------------------------------------------------------
# NOTE! We are using gcc 4.9 compiler but with std=c++11
# ------------------------------------------------------

# setup some paths for where to find stuff
# (used in some makefiles)
if [[ ${DEV_ENV} == 'WORK' ]]; then
  INSTALL_HOME=/ec/prod/exodus/dgt/local/exodus/user/mtdata/installs		# basic sintall stuff
  INSTALL_EXTRA_HOME=/ec/prod/exodus/dgt/local/exodus/user/ewetzha/installs	# extra packages
  INSTALL_TEST=/ec/prod/exodus/dgt/local/exodus/user/ewetzha/installs-test	# experimental gcc compiler

  # oracle stuff
  export ORACLE_HOME=/ec/sw/oracle/client/product/11.2.0.2/
else
  INSTALL_HOME=/home/hans/installs
  INSTALL_EXTRA_HOME=${INSTALL_HOME}
  INSTALL_TEST=${INSTALL_HOME}
fi

# build environment root
export ENV_ROOT=`pwd`

# this variable is for backwards compatibility in Makefiles
export PROJECT_ROOT=${ENV_ROOT}/..

# --------------------------------------
# flags for which standard of C++ to run
# --------------------------------------
#export STDFLAG='-std=gnu++1y'		# C++1y
#export STDFLAG='-std=c++1y'		# C++1y + gnu extension
export STDFLAG='-std=c++11'		# C++11

# must include boost stuff
export BOOST_INC=${INSTALL_HOME}/include
export BOOST_LIB=${INSTALL_HOME}/lib

# occi stuff
export OCCI_INC=${INSTALL_HOME}/include/occi
export OCCI_LIB=${INSTALL_HOME}/lib/occi

# amend ld library path
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${PROJECT_ROOT}/lib/lib:${PROJECT_ROOT}/lib/test

# amend exe library path
export PATH=${INSTALL_TEST}/bin:${INSTALL_EXTRA_HOME}/bin:${PATH}:${PROJECT_ROOT}/bin/bin:${PROJECT_ROOT}/bin/test
export LD_LIBRARY_PATH=${INSTALL_TEST}/lib:${INSTALL_TEST}/lib64:${INSTALL_EXTRA_HOME}lib:${LD_LIBRARY_PATH}:${PROJECT_ROOT}/lib

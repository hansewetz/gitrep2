#!/bin/bash

# --------------------------------------------------------------------------------------
# this script adds a copyright notice on all h, hpp, cc and Makefiles in this repository
# --------------------------------------------------------------------------------------

# check that project root exist
if [[ ! -d ${PROJECT_ROOT} ]]; then
  echo "ERROR: PROJECT_ROOT is not a directory or doe snot exist"
  exit 1
fi
# check that copyright notice is exactly 1 line
nlines=`wc -l ${CFILE}`
if (( ${lines} != 1 )); then
  echo "file ${CFILE} must have exactly one line"
  exit 1
fi
# file for COPYRIGHT
CFILE=${PROJECT_ROOT}/copyright/COPYRIGHT.TXT

# run script from ${PROJECT_ROOT}
echo "INFO: cd'ing to PROJECT_ROOT"
cd ${PROJECT_ROOT}
RET=$?
if [[ ${RET} -ne 0 ]]; then
  echo "ERROR: could not cd to PROJECT_ROOT"
  exit 1
fi

# loop through all source files in repository
echo "INFO: looping through all 'h,hpp,cc,Makefile' files ..."
FILES=`find . -type f -name '*.h' -or -name '*.cc' -or -name '*.hpp' -or -name Makefile`
COUNT=0
for file in ${FILES}; do
  echo "updating file ${file} ..."

  # remove first line of file and add copyright notice
  cp ${CFILE} ${file}.tmp
  echo >> ${file}.tmp
  echo >> ${file}.tmp
  cat ${file} | sed '1d' >> ${file}.tmp
  mv ${file}.tmp ${file}
done

# no errors so far
echo "INFO: updated ${COUNT} files - don't forget to compile and then commit
exit 0


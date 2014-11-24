#!/bin/bash

# -------------------------------------------------------------------------------------------------
# This scripts removes an old copy right notice in all h, hpp and cc files and inserts a new one
# -------------------------------------------------------------------------------------------------

# check that project root exist
if [[ ! -d ${PROJECT_ROOT} ]]; then
  echo "ERROR: PROJECT_ROOT is not a directory or doe snot exist"
  exit 1
fi
# file for COPYRIGHT
CFILE=${PROJECT_ROOT}/copyright/COPYRIGHT.TXT
CSTRING='Copyright Hans Ewetz'

# check that copyright notice is exactly 1 line
nlines=`wc -l ${CFILE}| awk '{print $1;}'`
echo "nlines: ${nlines}"
if (( ${nlines} != 1 )); then
  echo "file ${CFILE} must have exactly one line"
  exit 1
fi

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
FILES=`find . -type f -name '*.h' -or -name '*.cc' -or -name '*.hpp'`
COUNT=0
for file in ${FILES}; do
  echo "updating file ${file} ..."

  # count #of copyrights in the beginning of file
  count=`sed -n 1p ${file} | grep 'Copyright Hans Ewetz'|wc -l`

  # insert copyright notice if needed
  cp ${CFILE} ${file}.tmp                                  
  echo >>  ${file}.tmp                                      # insert a blank line after copyright
  if (( ${count} > 0 )); then
    cat ${file} |  awk 'NR>2{print}' >> ${file}.tmp         # remote copy right notice
  else
    cat ${file} | cat >> ${file}.tmp                        # no copy right notice found
  fi
  mv ${file}.tmp ${file}
done

# no errors so far
echo "INFO: updated ${COUNT} files - don't forget to compile and then commit"
exit 0


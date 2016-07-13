#!/usr/bin/env python

import sys

# generate subsets
# v - vector
# bv - bol vector, true of element part of subset, false otherwise
# k - current position
# n #of elements in v
def subsetsAux(v,bv,k,n):
  # check if we should print elements
  if k==n:
    for i in range(0,n):
      if bv[i]: sys.stdout.write("{0} ".format(v[i]));
    sys.stdout.write("\n")
  else:
    bv[k]=False
    subsetsAux(v,bv,k+1,n)
    bv[k]=True
    subsetsAux(v,bv,k+1,n)


# print subsets
def subsets(v):
  bv=[False]*len(v)
  subsetsAux(v,bv,0,len(v))

# test
v=[1,2,3]
print(v)
subsets(v)

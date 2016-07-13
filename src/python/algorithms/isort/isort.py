#!/usr/bin/env python

# execute insertion sort alg
def isort(v):
  vlen=len(v)
  for i in range(1,vlen):
    for j in range(i,0,-1):
      if v[j]<v[j-1]: v[j-1],v[j]=v[j],v[j-1]

# test
v=[6,3,4,9,12,45,0,2]
print(v)
isort(v)
print(v)

#!/usr/bin/env python

# execute selection sort alg
def selsort(v):
  vlen=len(v)
  for i in range(0,vlen):
    ismall=i
    for j in range(i+1,vlen):
      if v[j]<v[ismall]: ismall=j
    v[i],v[ismall]=v[ismall],v[i]

# test
v=[6,3,4,9,12,45,0,2]
print(v)
selsort(v)
print(v)

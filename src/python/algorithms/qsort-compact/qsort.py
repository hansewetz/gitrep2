#!/usr/bin/env python

# qsort
def qsortAux(v,l,u):
  if l>=u: return

  # partition
  m=l
  for i in range(l+1,u+1):
    if v[i]<=v[l]:
      v[i],v[m+1]=v[m+1],v[i]
      m=m+1
  v[l],v[m]=v[m],v[l]

  # recursivly qsort
  qsortAux(v,l,m)
  qsortAux(v,m+1,u)

# qsort wrapper
def qsort(v):
  qsortAux(v,0,len(v)-1)

# test
v=[6,3,4,9,12,45,0,2]
print(v)
qsort(v)
print(v)

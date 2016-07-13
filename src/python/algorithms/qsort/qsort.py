#!/usr/bin/env python

# partition vector
def part(v,l,u):
  m=l
  for i in (range(l+1,u+1)):
    if v[i]<=v[l]:
      v[i],v[m+1]=v[m+1],v[i]
      m=m+1
  v[l],v[m]=v[m],v[l]
  return m

# qsort a vector
def qsortAux(v,l,u):
  if l>=u: return
  m=part(v,l,u)
  qsortAux(v,l,m)
  qsortAux(v,m+1,u)

# qsort a vector
def qsort(v):
  qsortAux(v,0,len(v)-1)

# test
v=[6,3,4,9,12,45,0,2]
print(v)
qsort(v)
print(v)

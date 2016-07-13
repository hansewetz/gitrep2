#!/usr/bin/env python

# partition list 
def partAux(v,l,u):
  m=l
  for i in range(l+1,u+1):
    if v[i]<=v[l]:
      v[i],v[m+1]=v[m+1],v[i]
    m=m+1
  v[l],v[m]=v[m],v[l]
  return m

# partition list 
def part(v):
  return partAux(v,0,len(v)-1)

# test program
v=[7,4,2,8,10,18,0,2]
print(v)
print("part: {0}".format(part(v)))
print(v)

#!/usr/bin/env python

from collections import defaultdict
from collections import deque

# traverse graph in bfs order
def bfs(nstart, adjanecylist):
  visited = set()
  q = deque()
  q.appendleft(nstart)
  while q:
      ncurr = q.pop()
      if ncurr in visited: continue
      visited.add(ncurr)
      print ncurr
      q.extendleft(adjanecylist[ncurr])


# main test program
def main():
    # dict with default value as list
    adjanecylist = defaultdict(lambda : [])

    edges = [[1, 6], [6, 5], [1, 2], [1, 3], [2, 5], [3, 4], [4, 5]]
    for nstart, nend in edges:
        adjanecylist[nstart].append(nend)
    bfs(1, adjanecylist)


if __name__ == '__main__':
    main()

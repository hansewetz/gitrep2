#!/bin/env python

import efdbutils
import efqmanager

# -----------------  do work in txn --------------------
# create a qmanager
def dowork(con):
  qman=efqmanager.qmanager(con)
  print("msgcount: {0}".format(qman.msgcount(efqmanager.PROCESSED)))
  msg=qman.getmsgbyid(5)
  print("msg: "+str(msg))

# -----------------  main test program --------------------
# main test program
def main():
  con=None
  try:
    # setup db connection and create a txn
    con=efdbutils.opendb("test.db")
    txn=efdbutils.gettxnwrapper(con,10,True)

    # execute some operation
    txn(dowork)
  finally:
    if con is not None:efdbutils.closedb(con)

# execute if 'main'
if __name__=='__main__':main()

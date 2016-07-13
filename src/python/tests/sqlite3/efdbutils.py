import sqlite3 as lite

# ----------------- db mechanics --------------------

# open database
# (returns connection)
def opendb(dbname):
  return lite.connect(dbname)

# close database
def closedb(con):con.close()

# transaction wrapper
def txnwrapper(con,rollback,tmosec,f):
  try:
    cur=con.cursor()
    cur.execute("PRAGMA foreign_keys=ON;")
    cur.execute("PRAGMA busy_timeout = {0};".format(str(tmosec)))
    cur.execute("BEGIN IMMEDIATE TRANSACTION;")
    f(con)
    con.rollback if rollback else con.commit
  except:
    con.rollback()
    raise

# get a transaction wrapper
# (peel off all parameters except for the function to be called)
def gettxnwrapper(con,rollback,tmosec):
  return lambda f:txnwrapper(con,rollback,tmosec,f)

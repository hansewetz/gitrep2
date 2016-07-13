# ----------------- queue/message related method --------------------

# queue types
INBOUND=(0,"INBOUND")
PROCESSING=(1,"PROCESSING")
PROCESSED=(2,"PROCESSED")
EXCEPTION=(3,"EXCEPTION")
EXCEPTION_IGNORE=(4,"EXCEPTION_IGNORE")

# message class
class message():
  def __init__(self,id,type,corrid):
    self.id=id
    self.type=type
    self.corrid=corrid

  def __str__(self):
    return "id: "+str(self.id)+", type: "+str(self.type)+", corrid: "+str(self.corrid)

# queue manager
class qmanager:
  def __init__(self,con):self.con=con

  # get message count for a specifiv message id
  def msgcount(self,qtype):
    cur=self.con.execute("select count(1) from message where queue_type="+str(qtype[0]))
    return cur.fetchone()[0]

  # get message by id
  def getmsgbyid(self,id):
    cur=self.con.execute("select id,message_type,corr_id from message where id = "+str(id))
    row=cur.fetchone()
    return None if row==None else message(row[0],row[1],row[2])

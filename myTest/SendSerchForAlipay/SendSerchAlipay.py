#!/usr/bin/python2.4
# -*- coding:utf8 -*- 

import MySQLdb
import os
import time
import sys


DBIP = 'localhost'
DBUser = 'guoan'
DBPasswd = 'guoan'
DBName = 'WEBP'
DBPort = 3306
'''
#sql tmp
SELECT 
*
FROM t_trans_jnl 
	WHERE  
trans_date = '2015-12-16' AND (

(b3_pcode='170000' AND b34_telnumber NOT IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode ='00' AND b3_pcode='380000' ) AND
b34_telnumber  IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode='00' AND b3_pcode='170000' )) 

OR

( b3_pcode='280000' AND b34_telnumber NOT IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode ='00' AND b3_pcode='380000' ) AND
b34_telnumber  IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode IN ('','96','Z1') AND b3_pcode='280000' ))

); 

'''
def ExecuteSql(sql = ''):
	ListOfOrder = [] 
	ListTmp = []
	try:
		conn=MySQLdb.connect(host=DBIP,user=DBUser,passwd=DBPasswd,db=DBName,port=DBPort)
		cur = conn.cursor(MySQLdb.cursors.DictCursor )
		cur.execute(sql)
		result=cur.fetchall()
		print('their are %ld orders !!' % len(result))
		for each in result:
			ListTmp.append(each['mid'])
			ListTmp.append(each['b34_telnumber'])
			ListOfOrder.append(ListTmp )
			ListTmp = []
		cur.close()
		conn.close()
		return ListOfOrder 
	except MySQLdb.Error,e:
		print "Mysql Error %d: %s" % (e.args[0], e.args[1])
		return ListOfOrder 



def SerchAllNeedOrderNo():
	orderList = []
	#1、get localtime
	T = time.localtime()
	localDate = '%04d' % T[0] + '-'+'%02d' % T[1] +'-' + '%02d' % T[2]
	localTime = '%02d' % T[3] + '%02d' % T[4] + '%02d' % T[5] 
	OneHourBefor = '%02d' % (T[3]-1) + '%02d' % T[4]  + '%02d' % T[5]  
	print('today is :%s \nlocad time : %s ,one hour befor is %s ' % (localDate,localTime,OneHourBefor) )
	#2、get sql
	sql = 'select *from t_trans_jnl where b34_telnumber !=\'\' limit 10'
	#3、connect mysql and get result
	orderList = ExecuteSql(sql)
	if len(orderList) == 0 :
		print('their is no order need to send serch!!')
	#print(orderList)
	return orderList 


def SendSerch(orderlist = [] ):
	#print('orderlist = ', orderlist)
	if len(orderlist) == 0:
		return 0
	for each in orderlist:
		print each
	pass

if __name__ == '__main__':
	#1、Get all need Serch order number
	orderNolist = SerchAllNeedOrderNo()
	if len(orderNolist)  == 0:
		print('their is no Alipay order number need to serch!!')
		sys.exit(0)
	#2、send order serch of Aliapy
	ret = SendSerch(orderNolist)


	sys.exit(0)	 

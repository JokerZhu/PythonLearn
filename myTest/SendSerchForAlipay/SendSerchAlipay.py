#!/usr/bin/python2.4
# -*- coding:utf8 -*- 

import MySQLdb
import os
import time
import sys
import socket 
import md5
import logging
import datetime

DBIP = 'localhost'
DBUser = 'guoan'
DBPasswd = 'guoan'
DBName = 'WEBP'
DBPort = 3306

ServerIP = '192.168.1.90'
#ServerIP = '192.168.100.129'
ServerPORT = 6789 
#serch orders how many hours befor
Hourbefor = 1
#time quantum
Interval = 1

TEST = 0


def GetLogFileName():

	T = time.localtime()
	localDate = '%04d' % T[0] + '-'+'%02d' % T[1] +'-' + '%02d' % T[2]
	return sys.path[0]+'/AliTransSerch.' + localDate + '.log'


logging.basicConfig(level=logging.DEBUG,
    format='%(asctime)s [%(filename)s][line:%(lineno)d] [%(process)d] [%(levelname)s] %(message)s',                                                                     
       datefmt='[%Y%m%d%H%M%S]',
                filename=GetLogFileName() ,
                filemode='a+')

def ExecuteSql(sql = ''):
	ListOfOrder = [] 
	ListTmp = []
	try:
		conn=MySQLdb.connect(host=DBIP,user=DBUser,passwd=DBPasswd,db=DBName,port=DBPort)
		cur = conn.cursor(MySQLdb.cursors.DictCursor )
		cur.execute(sql)
		result=cur.fetchall()
		logging.info('their are %ld orders !!' % len(result))
		for each in result:
			ListTmp.append(each['mid'])
			ListTmp.append(each['b34_telnumber'])
			ListOfOrder.append(ListTmp )
			ListTmp = []
		cur.close()
		conn.close()
		return ListOfOrder 
	except MySQLdb.Error,e:
		logging.error( "Mysql Error %d: %s" % (e.args[0], e.args[1]))
		return ListOfOrder 



def SerchAllNeedOrderNo():
	orderList = []
	isInterDay = 0
	#1、get localtime
	if TEST == 1:
		localDate = '2015-12-12'
		Hour = 3
		Min = '0000'
		localTime =  '%02d' % Hour + Min 
		OneHourBefor =  '%02d' %(Hour -1) + Min 
	else:
		localTime = datetime.datetime.now()
		oneHourbefor = (localTime - datetime.timedelta(hours=Hourbefor)).strftime('%Y%m%d%H%M%S')
		twoHourbefor = (localTime - datetime.timedelta(hours=Hourbefor + Interval)).strftime('%Y%m%d%H%M%S')
		logging.info('serch between[ %s-%s-%s %s:%s:%s] and [%s-%s-%s %s:%s:%s] orders ' %(
			twoHourbefor[0:4],twoHourbefor[4:6],twoHourbefor[6:8],twoHourbefor[8:10],twoHourbefor[10:12],twoHourbefor[12:14],
			oneHourbefor[0:4],oneHourbefor[4:6],oneHourbefor[6:8],oneHourbefor[8:10],oneHourbefor[10:12],oneHourbefor[12:14]
			)
			)
		if oneHourbefor[0:8] == twoHourbefor[0:8]:
			isInterDay = 0
			logging.info('not Inter day')
		else:
			isInterDay = 1
			logging.info('Inter day')
	#logging.info('today is :%s \nlocad time : %s ,one hour befor is %s ' % (localDate,localTime,OneHourBefor) )
	#2、get sql
	if isInterDay == 1:
		sql = "SELECT * FROM t_trans_jnl WHERE (( trans_date = date('%s') and trans_time BETWEEN TIME('%s') and TIME('235959')) OR (trans_date = date('%s') and trans_time BETWEEN TIME('000000') and TIME('%s') )  ) AND ((b3_pcode='170000' AND b34_telnumber NOT IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode ='00' AND b3_pcode='380000' ) AND b34_telnumber  IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode='00' AND b3_pcode='170000' )) OR ( b3_pcode='280000' AND b34_telnumber NOT IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode ='00' AND b3_pcode='380000' ) AND b34_telnumber  IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode IN ('','96','Z1') AND b3_pcode='280000' )))" % (twoHourbefor[0:8],twoHourbefor[8:14] ,oneHourbefor[0:8],oneHourbefor[8:14])
	else:
		sql = "SELECT * FROM t_trans_jnl WHERE  trans_date = DATE('%s') and trans_time BETWEEN TIME('%s') and TIME('%s') AND ((b3_pcode='170000' AND b34_telnumber NOT IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode ='00' AND b3_pcode='380000' ) AND b34_telnumber  IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode='00' AND b3_pcode='170000' )) OR ( b3_pcode='280000' AND b34_telnumber NOT IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode ='00' AND b3_pcode='380000' ) AND b34_telnumber  IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode IN ('','96','Z1') AND b3_pcode='280000' )))" % (oneHourbefor[0:8],twoHourbefor[8:14],oneHourbefor[8:14])
	logging.info (sql)
	#3、connect mysql and get result
	orderList = ExecuteSql(sql)
	if len(orderList) == 0 :
		logging.warning('their is no order need to send serch!!')
	#logging.info(orderList)
	return orderList 

def PackSendData(sourceList = []):
	
	if len(sourceList) == 0:
		return None
	data =  '2000' + '000300' + '%04d' % len(sourceList[0]) + sourceList[0] + '0000' + '%04d' % len(sourceList[1][4:]) + sourceList[1][4:]
#	logging.info(data[4:])
	m = md5.new()
	m.update(data[4:] + '123')
	data += m.hexdigest()
	return data


def SendSerch(orderlist = [] ):
	#logging.info('orderlist = ', orderlist)
	i = 0
	if len(orderlist) == 0:
		return 0
	for each in orderlist:
		i += 1
		logging.info(each)
		logging.info('current the [%d] order ,orderno = [%s]'%(i,each[1]) )
		ret = PackSendData(each)
		if ret != None:
			SendData(ret)
			
		

#创建TCP链接函数
#功能:创建socket 并connect加密机
def CreatTcpConntion():
	global  fd 
	TimeOut = 5 
	#创建socket
	fd = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	fd.settimeout(TimeOut)
	#connect对端
	try:
		fd.connect((ServerIP,ServerPORT))
	except socket.gaierror, e:
		logging.error('1、can\'t connect your ip/port')
		return -1
	except socket.error, e:
		logging.error('2、can\'t connect your ip/port')
		return -1
	return 0


def SendData(data): 
	ret = CreatTcpConntion()
	if ret < 0 :
		logging.error('connect error ! ip = [%s],port = [%d]' % (ServerIP,ServerPORT))
		return None
	#Get 4 bite data len
	SendData = '%04d' % (len(data)) + data
	#fd.sendall('%04d' %(len(data)) )
	logging.info('send data : [%s]' % SendData)
	#send data
	ret = fd.sendall(SendData.encode())
	if ret != None:
		logging.error('send error!')
		fd.close()
		return None 
	#rev data len 
	try:
		recvDataLen = fd.recv(4)
	except socket.timeout,e:
		logging.error('rev data error:' ,e)
		fd.close()
		return None
	if int(recvDataLen) <= 0:
		logging.error('rev data len error')
		fd.close()
		return None
	#rev Data
	try:
		recvData = fd.recv(int(recvDataLen))
	except socket.timeout,e:
		logging.error('rev data error:',e)
		fd.close()
		return None
	logging.info('bakData = %s' % recvData)	
	
	fd.close()
	return recvData


if __name__ == '__main__':
	logging.info('\n================  start  ========================\n\n')
	#1、Get all need Serch order number
	orderNolist = SerchAllNeedOrderNo()
	if len(orderNolist)  == 0:
		logging.info('their is no Alipay order number need to serch!!')
		logging.info('\n================  end  ========================\n\n')
		sys.exit(0)
	#2、send order serch of Aliapy
	ret = SendSerch(orderNolist)


	logging.info('\n================  end  ========================\n\n')
	sys.exit(0)	 

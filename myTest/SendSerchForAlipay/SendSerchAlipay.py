#!/usr/bin/python2.4
# -*- coding:utf8 -*- 

import MySQLdb
import os
import time
import sys
import socket 
#import md5
import hashlib
import logging
import datetime
import ConfigParser
import re
'''
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
'''
'''
conf = ConfigParser.ConfigParser()
conf.read("config.ini")
sections = conf.sections()
#options = conf.options("env")
DBIP = conf.get("env", "DBIP")
DBUser = conf.get("env", "DBUser")
DBPasswd = conf.get("env", "DBPasswd")
DBName = conf.get("env", "DBName")
DBPort = conf.getint("env", "DBPort")

Hourbefor = conf.getint("variables", "Hourbefor")
Interval = conf.getint("variables", "Interval")
TEST = conf.getint("variables", "TEST")
'''
configName = sys.path[0]+'/config.ini'


def ReadConf(sections,option,type = 'string'):
	conf = ConfigParser.ConfigParser()
	result = None 
	if  (not isinstance(sections,str)) or (not isinstance(option,str)):
		logging.error('parameter error!!')
		return None
	try:
		conf.read(configName)
	except ConfigParser.Error ,e:
		logging.error(e)
	if type == 'string':
		try:
			result = conf.get(sections,option )
		except ConfigParser.NoSectionError , e:
			logging.error('can\'t find config sections:%s  option: %s,%s' %(sections,option,e)) 
		except ConfigParser.NoOptionError  , e:
			logging.error('can\'t find config sections:%s  option: %s' %(sections,option)) 
		return result
	elif type == 'int':
		try:
			result = conf.getint(sections,option )
		except ConfigParser.NoSectionError , e:
			logging.error('can\'t find config sections:%s  option: %s,%s' %(sections,option,e)) 
		except ConfigParser.NoOptionError  , e:
			logging.error('can\'t find config sections:%s  option: %s' %(sections,option)) 
		return result
	else:
		logging.error('error config type : %s' %(type)) 
		return result





DBIP = ReadConf("env", "DBIP")
DBUser = ReadConf("env", "DBUser")
DBPasswd = ReadConf("env", "DBPasswd")
DBName = ReadConf("env", "DBName")
DBPort = ReadConf("env", "DBPort",'int')
ServerIP = ReadConf("env", "ServerIP")
ServerPORT = ReadConf("env", "ServerPORT",'int')

Hourbefor = ReadConf("variables", "Hourbefor",'int')
Interval = ReadConf("variables", "Interval",'int')
TEST = ReadConf("variables", "TEST",'int')
Days = ReadConf("variables", "DAY",'int')
logName = ReadConf("variables", "logname")
#print(DBIP,DBUser,DBPasswd,DBName,DBPort,Hourbefor,Interval,TEST)
def GetLogFileName():

	T = time.localtime()
	localDate = '%04d' % T[0] + '-'+'%02d' % T[1] +'-' + '%02d' % T[2]
	return sys.path[0]+'/'+logName + '.'+ localDate + '.log'


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
	except TypeError,e:
		logging.error( "Mysql Error  %s" % (e))
		return ListOfOrder 



def SerchAllNeedOrderNo():
	orderList = []
	isInterDay = 0
	#1 get localtime
	if TEST == 1:
		localDate = '2015-12-12'
		Hour = 3
		Min = '0000'
		localTime =  '%02d' % Hour + Min 
		OneHourBefor =  '%02d' %(Hour -1) + Min 
	else:
		localTime = datetime.datetime.now()
		oneHourbefor = (localTime - datetime.timedelta(hours=Hourbefor)).strftime('%Y%m%d%H%M%S')
		twoHourbefor = (localTime - datetime.timedelta(hours=Hourbefor,minutes= Interval)).strftime('%Y%m%d%H%M%S')
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
	#2 get sql
	if isInterDay == 1:
		sql = "SELECT * FROM t_trans_jnl WHERE (( trans_date = date('%s') and trans_time BETWEEN TIME('%s') and TIME('235959')) OR (trans_date = date('%s') and trans_time BETWEEN TIME('000000') and TIME('%s') )  ) AND ((b3_pcode='170000' AND b34_telnumber NOT IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode ='00' AND b3_pcode='380000' ) AND b34_telnumber  IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode='00' AND b3_pcode='170000' )) OR ( b3_pcode='280000' AND b34_telnumber NOT IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode ='00' AND b3_pcode='380000' ) AND b34_telnumber  IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode IN ('','96','Z1') AND b3_pcode='280000' )))" % (twoHourbefor[0:8],twoHourbefor[8:14] ,oneHourbefor[0:8],oneHourbefor[8:14])
	else:
		sql = "SELECT * FROM t_trans_jnl WHERE  trans_date = DATE('%s') and trans_time BETWEEN TIME('%s') and TIME('%s') AND ((b3_pcode='170000' AND b34_telnumber NOT IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode ='00' AND b3_pcode='380000' ) AND b34_telnumber  IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode='00' AND b3_pcode='170000' )) OR ( b3_pcode='280000' AND b34_telnumber NOT IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode ='00' AND b3_pcode='380000' ) AND b34_telnumber  IN (SELECT b34_telnumber FROM t_trans_jnl WHERE  Channel='3006' AND b39_respcode IN ('','96','Z1') AND b3_pcode='280000' )))" % (oneHourbefor[0:8],twoHourbefor[8:14],oneHourbefor[8:14])
	logging.info (sql)
	#3 connect mysql and get result
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
	m = hash.md5()
	#m = md5.new()
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
		logging.error('1 can\'t connect your ip/port')
		return -1
	except socket.error, e:
		logging.error('2 can\'t connect your ip/port')
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

def CleanLogsOf(Befordays = 10):
	logging.info('ready to clean logs %d days ago'% Befordays)

	dateList = []
	localTime = datetime.datetime.now()
	'''
	today = localTime.strftime('%Y-%m-%d')
	for i in range(0,Befordays): 
		dateList.append((localTime - datetime.timedelta(days= Befordays+i)).strftime('%Y-%m-%d'))
	logging.info('today is  %s' % today)
	logging.info(dateList)
	'''
	for n in os.listdir(sys.path[0]):
		a = os.path.join(sys.path[0],n)
		if os.path.isfile(a):
			filename = n.split('.')
			#logging.info(filename)
			if (len(filename) == 3) and (filename[0] == logName ) and (filename[2] == 'log') :
				#logging.info(filename[1])
				#logging.info('%s %s %s' % (filename[1][0:4],filename[1][5:7],filename[1][8:10] ))
				if ( datetime.date.today() - datetime.date(int(filename[1][0:4]),int(filename[1][5:7]),int(filename[1][8:10])) ).days > Days:
					logging.info('rm log file: %s' % n)
					os.remove(a)

	logging.info('clean logs of %d days ago end'% Befordays)


if __name__ == '__main__':
	logging.info('\n================  start  ========================\n\n')
	#1 clean logs of many days ago 
	CleanLogsOf(Days)
	#2 Get all need Serch order number
	orderNolist = SerchAllNeedOrderNo()
	if len(orderNolist)  == 0:
		logging.info('their is no Alipay order number need to serch!!')
		logging.info('\n================  end  ========================\n\n')
		sys.exit(0)
	#3 send order serch of Aliapy
	ret = SendSerch(orderNolist)
	'''
	str1 =  ReadConf('eenv','test','dou')
	logging.info('str1 = %s' % str1)

	'''
	logging.info('\n================  end  ========================\n\n')
	sys.exit(0)	 

#!/usr/bin/python3.4
# -*- coding:utf8 -*- 
#import MySQLdb
import pymysql as MySQLdb
import os
import time
import sys
import socket 
import logging
import datetime
try:
	import ConfigParser 
except  ImportError:
	import configparser as ConfigParser
import re

from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email import encoders
from email.header import Header
from email.utils import parseaddr, formataddr
from email.mime.base import MIMEBase 
import smtplib
import xlwt

configName = sys.path[0]+'/config.ini'

def ReadConf(sections,option,type = 'string'):
	conf = ConfigParser.ConfigParser()
	result = None 
	if  (not isinstance(sections,str)) or (not isinstance(option,str)):
		logging.error('parameter error!!')
		return None
	try:
		conf.read(configName)
	except ConfigParser.Error as e:
		logging.error(e)
	if type == 'string':
		try:
			result = conf.get(sections,option )
		except ConfigParser.NoSectionError as e:
			logging.error('can\'t find config sections:%s  option: %s,%s' %(sections,option,e)) 
		except ConfigParser.NoOptionError  as e:
			logging.error('can\'t find config sections:%s  option: %s' %(sections,option)) 
		return result
	elif type == 'int':
		try:
			result = conf.getint(sections,option )
		except ConfigParser.NoSectionError as e:
			logging.error('can\'t find config sections:%s  option: %s,%s' %(sections,option,e)) 
		except ConfigParser.NoOptionError  as e:
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

TEST = ReadConf("variables", "TEST",'int')
Days = ReadConf("variables", "logCleanDays",'int')
logName = ReadConf("variables", "logname")
xlName = ReadConf("variables", "xlName")
recordDays = ReadConf("variables", "recordDays",'int')


def GetLogFileName():

	T = time.localtime()
	localDate = '%04d' % T[0] + '-'+'%02d' % T[1] +'-' + '%02d' % T[2]
	return sys.path[0]+'/'+logName + '.'+ localDate + '.log'


logging.basicConfig(level=logging.DEBUG,
    format='%(asctime)s [%(filename)s][line:%(lineno)d] [%(process)d] [%(levelname)s] %(message)s',                                                                     
       datefmt='[%Y%m%d%H%M%S]',
                filename=GetLogFileName() ,
                filemode='a+')
def CleanLogsOf(Befordays = 10):
	logging.info('ready to clean logs %d days ago'% Befordays)

	dateList = []
	localTime = datetime.datetime.now()
	for n in os.listdir(sys.path[0]):
		a = os.path.join(sys.path[0],n)
		if os.path.isfile(a):
			filename = n.split('.')
			#logging.info(filename)
			if (len(filename) == 3) and (filename[0] == logName ) and (filename[2] == 'log') :
				if ( datetime.date.today() - datetime.date(int(filename[1][0:4]),int(filename[1][5:7]),int(filename[1][8:10])) ).days > Days:
					logging.info('rm log file: %s' % n)
					os.remove(a)

	logging.info('clean logs of %d days ago end'% Befordays)


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
			'''
			ListTmp.append(each['mid'])
			ListTmp.append(each['b34_telnumber'])
			'''
			ListOfOrder.append(each )
			#ListTmp = []
		cur.close()
		conn.close()
		return ListOfOrder 
	except MySQLdb.Error as e:
		logging.error( "Mysql Error %d: %s" % (e.args[0], e.args[1]))
		return ListOfOrder 
	except TypeError as e:
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
		sql = "SELECT * FROM t_trans_jnl WHERE trans_date = CURDATE() - %d AND b39_respcode ='00' AND type NOT in ('E') AND \
			((msgcode  IN ('0200')) AND (b3_pcode  IN ('000000','510000','810000'))) AND \
			b37_refnumber='' limit 100 ;" % (recordDays)
		
	#logging.info('today is :%s \nlocad time : %s ,one hour befor is %s ' % (localDate,localTime,OneHourBefor) )
	#2 get sql
	logging.info (sql)
	#3 connect mysql and get result
	orderList = ExecuteSql(sql)
	if len(orderList) == 0 :
		logging.warning('their is no order need to send serch!!')
	#logging.info(orderList)
	return orderList 

def _format_addr(s):
	name,addr = parseaddr(s)
	return formataddr((Header(name,'utf-8').encode(),addr))


def sendEmail(fromAddr,passWD,toAddr,smtpServer,msg='',Subject='',file=''):
	msgMain = MIMEMultipart()
	msg = MIMEText(msg,'plain','utf-8')
	msgMain.attach(msg)
	#read to send
	msgMain['From'] = _format_addr('<%s>' % fromAddr)
	msgMain['To'] = toAddr
	msgMain['Subject'] = Header(Subject,'utf-8').encode()
	
	#file
	logging.info(file)
	try:
		with open(file,'rb') as fileSend:
			mime = MIMEBase('file', 'xls', filename=file)
			mime.add_header('Content-Disposition', 'attachment', filename=file)
			mime.add_header('Content-ID', '<0>')
			mime.add_header('X-Attachment-Id', '0')
			mime.set_payload(fileSend.read())
			encoders.encode_base64(mime)
			msgMain.attach(mime)
	except FileNotFoundError as e:
		logging.info('can\'t find the file[%s]' % file)

	#server init
	server = smtplib.SMTP(smtpServer,25)
	server.set_debuglevel(1)
	#login
	server.login(fromAddr,passWD)
	#server.sendmail(fromAddr,toAddr.split(';'),msg.as_string())
	#send the email
	server.send_message(msgMain)
	server.quit()

def sendErrorEmail(FILE = ''):
	msg = 'there are some errors in server'
	Subject = 'errors'
	#smtp server
	smtpServer = ReadConf("emailInfo", "smtpServer")
	From = ReadConf("emailInfo", "userId")
	Passwd = ReadConf("emailInfo", "passWD")
	To = ReadConf("emailInfo", "emailTo")
	logging.info(To)
	sendEmail(From,Passwd,To,smtpServer,msg,Subject,FILE)
def getXlName():
	global xlName
	if xlName == None:
		xlName = ReadConf("variables", "xlName")
	if xlName == None:
		logging.error('get excl name error')

	T = time.localtime()
	logging.info(T)
	localDate = '%02d' % T[1] + '%02d' % T[2] + '%02d' % T[3] + '%02d' % T[4] + '%02d' % T[5]
	return xlName + '_'+ localDate + '.xls'
	pass

def saveInExcl(recordlist):
	if isinstance(recordlist,list) :
		if len(recordlist) == 0:
			logging.info('no result')
			return -1
	else:
		logging.info('err fromat')
		return -1 
	xlName = getXlName()
	logging.info(xlName)
	wb = xlwt.Workbook(encoding='utf-8')
	ws = wb.add_sheet('A Test Sheet')

	k = 0
	for j in recordlist[0].keys():
		k += 1
		ws.write(0, k, str(j))

	for i in range(0,len(recordlist)):
		logging.info(recordlist[i])
		#for k,v in recordlist[i].items():
			#logging.info(str(k) +'='+ str(v))
		k = 0
		for j in recordlist[i].keys():
			k += 1
			logging.info(recordlist[i][j])
			ws.write(i+1, k, str(recordlist[i][j]))

	wb.save(xlName)
	return 0,xlName


	
	


if __name__ == '__main__':
	logging.info('start this......')
	ret = 0
	xlsName = ''
	#clean logs of many days ago 
	CleanLogsOf(Days)
	#search Suspected ShortAccount trans
	recordlist = SerchAllNeedOrderNo()
	logging.info(len(recordlist))
	#拿到可能短账的交易后，做一些简单的判断筛选

	#短账几率较大的交易记录做成文件
	if isinstance(recordlist,list) and len(recordlist) >= 0:
		ret,xlsName = saveInExcl(recordlist)
	else:
		ret = -1

	#附件发邮件通知特定人员
	if isinstance(ret,int) and  ret >= 0:
		sendErrorEmail( xlsName)
	print('end')
	logging.info('end this......')

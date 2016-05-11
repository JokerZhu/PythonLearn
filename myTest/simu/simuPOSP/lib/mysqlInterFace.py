#!/usr/bin/python3.4
# -*- coding:utf8 -*- 
import pymysql
import os
import time
import sys
import socket 
import logging
import myConf
import pack8583

TermInfo = {}


def ConnMysql():
	try:
		global conn
		global cur
		conn=pymysql.connect(host=myConf.DBIP,user=myConf.DBUser,
				passwd=myConf.DBPasswd,db=myConf.DBName,port=myConf.DBPort,cursorclass=pymysql.cursors.DictCursor)
		cur = conn.cursor(pymysql.cursors.DictCursor )
		cur = conn.cursor( )
	except pymysql.err.Error as e:
		logging.error( "mysql error  %s" % (e))
		return -1 
	return 1;

def ReConnMysql():
	logging.info('ReConning Mysql')
	disconnMysql()
	try:
		global conn 
		global cur
		conn=pymysql.connect(host=myConf.DBIP,user=myConf.DBUser,
				passwd=myConf.DBPasswd,db=myConf.DBName,port=myConf.DBPort,cursorclass=pymysql.cursors.DictCursor)
		cur = conn.cursor(pymysql.cursors.DictCursor )
		cur = conn.cursor( )
	except pymysql.err.Error as e:
		logging.error( "mysql error  %s" % (e))
		return -1 
	return 1;


def disconnMysql():
	try:
		cur.close()
		conn.close()
	except pymysql.err.Error as  e:
		logging.error( "mysql error %s" % (e))
	pass


def RunSelectSql(sql = ''):
	if (not isinstance(sql,str)):
		return None
	else:
		logging.info('sql:[%s]' %sql)
	try:
		conn.ping()
	except:
		ReConnMysql()
	try:
		cur.execute(sql)
		result = cur.fetchall()
	except pymysql.err.Error as e:
		logging.error('execute sql error [%s]' % e)
		return None 
	logging.info('sql execute ok')
	if len(result) == 0:
		return 0

	return result
def RunSql(sql = ''):
	if (not isinstance(sql,str)):
		return None
	else:
		logging.info('sql:[%s]' %sql)
	try:
		result = cur.execute(sql)
	except pymysql.err.Error as e:
		logging.error('execute sql error [%s]' % e)
		return 0 
	logging.info('sql execute ok')

	return result

	pass
	
def SelFormDB(Flds = '',From = '' ,Where = '' ):
	if (not isinstance(Flds,str)) or (not isinstance(From,str)) or \
		(not isinstance(Where,str)):
		logging.error('parameter error!!')
		return None
	sqlstr = 'select ' + Flds
	sqlstr += ' from ' + From
	if len(Where) > 0:
		sqlstr += ' where ' + Where
	#print(sqlstr)
	return RunSelectSql(sqlstr)
	pass

def UpdateToDB(Flds = '',From = '' ,Where = ''):
	if (not isinstance(Flds,str)) or (not isinstance(From,str)) or \
		(not isinstance(Where,str)):
		logging.error('parameter error!!')
		return 0 
	if len(Flds) == 0 or len(From) == 0 or len(Where) == 0: 
		logging.error('parameter error!!')
		return 0 


	sqlstr = 'update ' + From 
	sqlstr += ' set ' + Flds
	sqlstr += ' where ' + Where
	#print(sqlstr)
	return RunSql(sqlstr)
	pass

def InsertToDB( Flds ='',From = ''):
	if (not isinstance(Flds,str)) or (not isinstance(From,str)):
		logging.error('parameter error!!')
		return 0 
	if len(Flds) == 0 or len(From) == 0:
		logging.error('parameter error!!')
		return 0 


	sqlstr = 'insert into ' + From 
	sqlstr +=  ' ' + Flds
	print(sqlstr)
	return RunSql(sqlstr)

def ReadTermInfo(mid = '',tid = '',type = '' ):
	global TermInfo
	if type == 'R':
		logging.info('reselect terminfo')
	elif len(TermInfo) > 0:
		logging.info('alreadly have value')
		return True

	if (not isinstance(mid,str)) or (not isinstance(tid,str)):
		logging.error('parameter error!!')
		return False
	if len(mid) == 0 or len(tid) == 0:
		logging.error('parameter error!!')
		return False

	result = SelFormDB('*','term' ,' mid = \'%s\' and tid = \'%s\' limit 1'\
		 % (mid,tid) )
	if result == 0:
		return False 
	else:
		TermInfo = result[0]
		return True
	
	
	pass
'''
while the req,insert the Transaction flow
'''
def InsertTransReq():
	tableMapping = {
		0:'b0',
		2:'b2',
		3:'b3',
		4:'b4',
		7:'b7',
		11:'b11',
		12:'b12',
		13:'b13',
		14:'b14',
		15:'b15',
		21:'b21',
		22:'b22',
		23:'b23',
		25:'b25',
		26:'b26',
		32:'b32',
		35:'b35',
		36:'b36',
		37:'b37',
		39:'b39',
		41:'b41',
		42:'b42',
		49:'b49',
		52:'b52',
		54:'b54',
		60:'b60',
		61:'b61',
		62:'b62',
		64:'b64',
	}
	flds = ''
	for i in range(0,64):
		try:
			if len(flds) == 0:
				flds += tableMapping[i] + ' = \'' + pack8583.package[i].decode() + '\'' 
			else:
				flds +=',' + tableMapping[i] + ' = \'' + pack8583.package[i].decode() + '\'' 
		except KeyError:
			continue
	#增加时间戳
	flds +=',' + 'localTransTime = ' + str(time.time()*1000000 )
	print(flds)
	InsertToDB(' set ' + flds,'trans' )
	pass

'''
befor response,update trans stat
'''
def UpdateTransRes():
	where = ''
	flds = ''
	whereMapping = {
		0:'b0',
		3:'b3',
		11:'b11',
		41:'b41',
		42:'b42',
	}
	updateFldMapping = {
		12:'b12',
		13:'b13',
		14:'b14',
		15:'b15',
		37:'b37',
		39:'b39',
	}
	try:
		if pack8583.packageRes[0][2] == '1':
			pack8583.packageRes[0] = pack8583.packageRes[0][:2] + '0' + pack8583.packageRes[0][-1]
	except AttributeError:
		if pack8583.packageRes[0].decode()[2] == '1':
			pack8583.packageRes[0] = pack8583.packageRes[0][:2] + '0' + pack8583.packageRes[0][-1]
	except KeyError:
		logging.error('error in get fld 0')
		
	#print(pack8583.package)
	for i in whereMapping.keys():
		try :
			if len(where) == 0:
				try:
					where += whereMapping[i] + '=\'' + pack8583.packageRes[i].decode() + '\''
				except AttributeError:
					where += whereMapping[i] + '=\'' + pack8583.packageRes[i] + '\''
			else:
				try:
					where += ' and ' + whereMapping[i] + '=\'' + pack8583.packageRes[i].decode() + '\''
				except AttributeError:
					where += ' and ' + whereMapping[i] + '=\'' + pack8583.packageRes[i] + '\''
		except KeyError:
			continue
	#print(where)

	for i in updateFldMapping.keys():
		try :
			if len(flds) == 0:
				try:
					flds +=updateFldMapping[i] + ' = \'' + pack8583.packageRes[i].decode() + '\'' 
				except AttributeError:
					flds +=updateFldMapping[i] + ' = \'' + pack8583.packageRes[i] + '\'' 
			else:
				try:
					flds +=',' +updateFldMapping[i] + ' = \'' + pack8583.packageRes[i].decode() + '\'' 
				except AttributeError:
					flds +=',' +updateFldMapping[i] + ' = \'' + pack8583.packageRes[i] + '\'' 
		except KeyError:
			continue
	#print(flds)
	if len(where) == 0 or len(flds) == 0:
		return 0
	else:
		where += ' ORDER BY localTransTime DESC LIMIT 1'
		UpdateToDB(flds,'trans',where )
		

#ConnMysql()
#SelFormDB('*','term','tid = \'00000002\'')
#InsertToDB('(mid,tid,tmk,status) values (\'812331545110014\',\'33150022\',\'8EBB00D03EAD89148EBB00D03EAD8914\',\'0\')','term' )
#print(ReadTermInfo('000000000000001','00000001'))

#print(ReadTermInfo('000000000000001','00000001','R'))
#print(TermInfo['tmk'])
#disconnMysql()

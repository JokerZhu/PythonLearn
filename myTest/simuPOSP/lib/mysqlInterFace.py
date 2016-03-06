#!/usr/bin/python3.4
# -*- coding:utf8 -*- 
#import MySQLdb
import pymysql
import os
import time
import sys
import socket 
import logging
import myConf




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
		cur.execute(sql)
		result = cur.fetchall()
	except pymysql.err.Error as e:
		logging.error('execute sql error [%s]' % e)
		return None 

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
	print(sqlstr)
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
	print(sqlstr)
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



ConnMysql()
InsertToDB('(mid,tid,tmk,status) values (\'000000000000001\',\'00000001\',\'8EBB00D03EAD89148EBB00D03EAD8914\',\'0\')','term' )
disconnMysql()

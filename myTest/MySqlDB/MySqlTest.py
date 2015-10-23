#!/usr/bin/python3.4
# -*- coding:utf-8 -*-
import MySQLdb

try:
	conn = MySQLdb.connect(host='127.0.0.1',user='guoan',passwd='guoan',db='MyTestDB',port=3306)
	cur = conn.cursor()
	cur.execute('select *from test_zhu')
	cur.close()
	conn.close()
except MySQLdb.Error as e:
	print('Mysql Error %d : %s ' % (e.args[0],e.args[1]))






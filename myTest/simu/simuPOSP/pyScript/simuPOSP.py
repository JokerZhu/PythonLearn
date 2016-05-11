#!/usr/local/bin/python3.4
#-*- coding:utf8 -*-
import sys
import os
homedir = sys.path[0] 
print('homedir = ',homedir)
sys.path.append(homedir + '/../lib')
from ctypes import *
import logging
import re
import socket
import binascii
import configparser
import time
import myConf
import customizeFun
import Security
import pack8583
import hsmSvr
import mysqlInterFace

import socketserver
from socketserver import (StreamRequestHandler as SRH,ForkingMixIn as FMI ,TCPServer as TCP)


#CheckMac flag
CheckMac = 0
GenMac = 0

def bcdhexToaschex(bcdHex ):
	ascHex =  ''.join(map(lambda x : '%02X' % ord(chr(x)),list(bcdHex))) 
	#logging.info('[backData = [%s]]' % ascHex)
	return ascHex
def transHandle():
	Indata = pack8583.package
	transType = ''
	try:
		if Indata[0] == b'0800':
			transType = '签到'
		elif Indata[0] == b'0900':
			transType = '机构签到'
		elif Indata[0] == b'0200' and Indata[3] == b'000000':
			transType = '消费'
		elif Indata[0] == b'0200' and Indata[3] == b'310000':
			transType = '余额查询'
		elif Indata[0] == b'0200' and Indata[3] == b'200000':
			transType = '撤销'
		elif Indata[0] == b'0400' and Indata[3] == b'000000':
			transType = '消费冲正'
		elif Indata[0] == b'0400' and Indata[3] == b'200000':
			transType = '撤销冲正'
		else:
			logging.error('unknown transtype')
			transType = None
	except KeyError: 
		transType = None
	logging.info('transType = [%s]' % transType)
	return transType
	pass


class ServerFMI(FMI,TCP):
	pass

class MyRequestHandler(SRH):
	def handle(self):
		#setting socket timeout
		self.request.settimeout(myConf.TimeOut )
		#recv data len
		try:
			dataLen = self.request.recv(2)
		except socket.timeout as e:
			logging.info("error data:[%s]" % e)
			return None
		length = int(dataLen[0]* 256 +dataLen[1])
		if length <= 0:
			logging.info("error data")
			return None
		logging.info('rev data len = [%d]' % length)
		#recv 8583
		try:
			data = self.request.recv(length)
		except socket.timeout as e:
			logging.error('recv data error [%s]' % e)
			return None
		#unpack 8583
		data = bytes(bcdhexToaschex(data[myConf.HeaderLen: ]).encode())
		logging.info("recv data = [%s] type = [%s]" % (data ,type(data)))
		result = pack8583.unpack8583(data)
		if result is None:
			exit(-1)
		logging.info(result)
		#Insert DB
		#mysqlInterFace.InsertTransReq()
		#trans handle
		#checkMac
		if CheckMac :
			if not (pack8583.package[0] == b'0800' or pack8583.package[0] == b'0900') :
				Security.CheckTermMacPOSP(data.decode())

		#try:
		transType = transHandle()
		if transType is None :
			exit(-1)

		#pack return 8583
		backData = pack8583.packPackage8583(transType)
		#Gen Mac
		Mac = [] 
		print(pack8583.package[0])
		if GenMac :
			if not (pack8583.package[0] == b'0800' or pack8583.package[0] == b'0900'):
				Mac = Security.GenTermMACForReturn(backData[0:len(backData) - 16].decode())
			if isinstance(Mac,list) and len(Mac) > 0 and Mac[0] == '00':
				backData = backData[0:len(backData) - 16] + binascii.hexlify(Mac[2].encode('iso-8859-15'))[0:16]
		logging.info(backData)
		backData = binascii.a2b_hex(bytes(myConf.packageHeader.encode('iso-8859-15'))) +  binascii.a2b_hex(backData)
		logging.info(backData)
		logging.info(len(backData))
		#update DB
		#mysqlInterFace.UpdateTransRes()
		#sendBack
		#time.sleep(10)
		ret = self.request.send(('%c%c' %(chr(int(len(backData)/256)),chr(int(len(backData)%256))) ).encode('iso-8859-15' ) + bytes(backData))


if __name__ == '__main__':
	logging.info('---------------------------------------start-------------------------------------------------')
	print('-' * 20)
	#连接加密机
	print('connecting HSM')
	if hsmSvr.CreatHsm() < 0:
		print('HSM connect error')
		exit(-1)
	else:
		print('HSM connect OK')
	print('-' * 20)
	#连接数据库
	print('connecting mysql')
	if mysqlInterFace.ConnMysql() < 0:
		print('mysql connect error')
		exit(-1)
	else:
		print('mysql connect OK')
	#加载交易返回定义文件
	myConf.loadAllTransTypecfg()
	#listen the port
	print('-' * 20)
	print('starting server')
	try:
		server = ServerFMI((myConf.MyIp,myConf.MyPort),MyRequestHandler)
		print('server start OK ')
		print('-' * 20)
		server.serve_forever()
	except OSError as e:
		print('server start failed [%s]' % e)
		logging.error('server start failed [%s]' % e)
		#hsmSvr.disConnHsm()
		mysqlInterFace.disconnMysql()
	except KeyboardInterrupt as e:
		print('you have stopped the simuPOSP')
		#hsmSvr.disConnHsm()
		mysqlInterFace.disconnMysql()

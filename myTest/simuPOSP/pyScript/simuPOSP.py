#!/usr/bin/python3.4
#-*- coding:utf8 -*-
import sys
from ctypes import *
import logging
import os
import re
import socket
import binascii
import configparser
import time
sys.path.append('../lib')
import customizeFun
import myConf
import Security
import pack8583
import hsmSvr
import mysqlInterFace

import socketserver
from socketserver import (StreamRequestHandler as SRH,ForkingMixIn as FMI ,TCPServer as TCP)


def bcdhexToaschex(bcdHex ):
	ascHex =  ''.join(map(lambda x : '%02X' % ord(chr(x)),list(bcdHex))) 
	#logging.info('[backData = [%s]]' % ascHex)
	return ascHex
def transHandle(Indata = {}):
	transType = ''
	if Indata[0] == b'0800' and Indata[3] == None:
		transType = '签到'
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
		dataLen = self.request.recv(2)
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
		resultList = pack8583.unpack8583(data)
		if resultList is None:
			exit(-1)
		logging.info(resultList)
		#trans handle
		#checkMac

		#try:
		transType = transHandle(resultList)
		if transType is None :
			exit(-1)

		#pack return 8583
		backData = pack8583.packPackage8583(transType,resultList)

		
		#self.request.send(data)
		#sendBack
		ret = self.request.send(('%c%c' %(chr(int(len(backData)/256)),chr(int(len(backData)%256))) ).encode() + bytes(backData))



if __name__ == '__main__':
	logging.info('---------------------------------------start-------------------------------------------------')
	#print(hsmSvr.SendData("K2S0018S0018Y316CBBC7EE8C9DC2D285580AC582A0EEY"))
	hsmSvr.CreatHsm()
	mysqlInterFace.ConnMysql()
	#print(hsmSvr.GenTermPinKey('8EBB00D03EAD89148EBB00D03EAD8914'))
	#print(hsmSvr.GenTermMacKey('8EBB00D03EAD89148EBB00D03EAD8914'))
	#hsmSvr.fd.close()

	#listen the port
	try:
		server = ServerFMI((myConf.MyIp,myConf.MyPort),MyRequestHandler)
		server.serve_forever()
	except OSError as e:
		print('server start failed')
		logging.error('server start failed [%s]' % e)
	except KeyboardInterrupt as e:
		print('you have stopped the simuPOSP')
		hsmSvr.disConnHsm()
		mysqlInterFace.disconnMysql()
	pass

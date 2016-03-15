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
def transHandle():
	Indata = pack8583.package
	transType = ''
	if Indata[0] == b'0800':
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
		result = pack8583.unpack8583(data)
		if result is None:
			exit(-1)
		logging.info(result)
		#trans handle
		#checkMac
		if pack8583.package[0] != b'0800':
			Security.CheckTermMacPOSP(data.decode())

		#try:
		transType = transHandle()
		if transType is None :
			exit(-1)

		#pack return 8583
		backData = pack8583.packPackage8583(transType)
		#Gen Mac
		if pack8583.package[0] != b'0800' or pack8583.package[0] != b'0810':
			Mac = Security.GenTermMacPOSP(backData[0:len(backData) - 16].decode())
		if isinstance(Mac,list) and Mac[0] == '00':
			backData = backData[0:len(backData) - 16] + binascii.hexlify(Mac[2].encode())[0:16]
		logging.info(backData)
		#backData = binascii.a2b_hex((bytes(myConf.packageHeader.encode()) + backData))
		#backData = binascii.a2b_hex(myConf.packageHeader.encode()) + binascii.a2b_hex(backData)
		backData = binascii.a2b_hex(bytes(myConf.packageHeader.encode())) +  binascii.a2b_hex(backData)
		#backData = bytes(myConf.packageHeader.encode()) + backData
		logging.info(backData)
		logging.info(len(backData))
		#logging.info('%c%c' %(chr(int(len(backData)/256)),chr(int(len(backData)%256))) )
		#self.request.send(data)
		#sendBack
		#ret = self.request.send(('%c%c' %(chr(int(len(backData)/256)),chr(int(len(backData)%256)))) + backData)
		ret = self.request.send(('%c%c' %(chr(int(len(backData)/256)),chr(int(len(backData)%256))) ).encode('iso-8859-15' ) + bytes(backData))


if __name__ == '__main__':
	logging.info('---------------------------------------start-------------------------------------------------')
	#print(hsmSvr.SendData("K2S0018S0018Y316CBBC7EE8C9DC2D285580AC582A0EEY"))
	print('-' * 20)
	print('connecting HSM')
	if hsmSvr.CreatHsm() < 0:
		print('HSM connect error')
		exit(-1)
	else:
		print('HSM connect OK')
	
	print('-' * 20)
	print('connecting mysql')
	if mysqlInterFace.ConnMysql() < 0:
		print('mysql connect error')
		exit(-1)
	else:
		print('mysql connect OK')

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
		hsmSvr.disConnHsm()
		mysqlInterFace.disconnMysql()
	except KeyboardInterrupt as e:
		print('you have stopped the simuPOSP')
		hsmSvr.disConnHsm()
		mysqlInterFace.disconnMysql()

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

import socketserver
from socketserver import (StreamRequestHandler as SRH,ForkingMixIn as FMI ,TCPServer as TCP)

def SendData(data): 
	ret = CreatTcpConntion()
	if ret < 0 :
		logging.error('connect error ! ip = [%s],port = [%d]' % (myConf.ServerIp,myConf.ServerPort))
		return None
	#Get 4 bite data len
	ret = fd.sendall(('%c%c' %(chr(int(len(data)/256)),chr(int(len(data)%256))) ).encode() + data )
	#send data
	#ret = fd.sendall(data)
	if ret != None:
		logging.error('send error!')
		fd.close()
		return None 
	else:
		logging.info('send data OK !')
	#rev data len 
	try:
		recvDataLen = fd.recv(2)
	except socket.timeout as e:
		logging.error('rev data error:%s' % e)
		fd.close()
		return None
	if int(recvDataLen[0]* 256 + recvDataLen[1]) <= 0:
		logging.error('rev data len error')
		fd.close()
		return None
	else:
		logging.info('recv data  len OK len = [%d] !' % (recvDataLen[0]* 256 + recvDataLen[1]))
	#rev Data
	try:
		recvData = fd.recv(recvDataLen[0]* 256 + recvDataLen[1] )
	except socket.timeout as e:
		logging.error('rev data error: %s ' % e)
		fd.close()
		return None
	logging.info('recv data = [%s]' % (recvData ))
	fd.close()
	#return recvData
	#logging.info(recvData[int(len(myConf.packageHeader)/2):])
	return bytes(bcdhexToaschex(recvData[int(len(myConf.packageHeader)/2):] ).encode())

def bcdhexToaschex(bcdHex ):
	ascHex =  ''.join(map(lambda x : '%02X' % ord(chr(x)),list(bcdHex))) 
	#logging.info('[backData = [%s]]' % ascHex)
	return ascHex


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
		
		#trans handle

		#pack return 8583
		print (data.decode())
		#send back
		self.request.send(data)



if __name__ == '__main__':
	logging.info('---------------------------------------start-------------------------------------------------')
	#listen the port
	try:
		server = ServerFMI((myConf.MyIp,myConf.MyPort),MyRequestHandler)
		server.serve_forever()
	except OSError as e:
		print('server start failed')
		logging.error('server start failed [%s]' % e)
	pass

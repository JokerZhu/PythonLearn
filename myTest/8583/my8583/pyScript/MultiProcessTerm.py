#!/usr/bin/python3.4
#-*- coding:utf8 -*-
import sys
#from ctypes import *
import logging
import os
import re
import socket
import binascii
import configparser
import time
sys.path.append('../lib')
import customizeFun
import pack8583
import myConf
import Security
import multiprocessing

#创建TCP链接函数
#功能:创建socket 并connect
def CreatTcpConntion():
	global  fd 
	#创建socket
	fd = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	fd.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) 
	fd.settimeout(myConf.TimeOut)
	#connect对端
	try:
		fd.connect((myConf.ServerIp,myConf.ServerPort))
	except socket.gaierror as e:
		logging.error('1 can\'t connect your ip/port')
		return -1
	except socket.error as e:
		logging.error('2 can\'t connect your ip/port')
		return -1
	logging.info('connect ip = [%s],port = [%d] OK !' % (myConf.ServerIp,myConf.ServerPort))
	return 0

def SendData(data): 
	start = time.time()
	logging.info(" starttime = [%.6f] " %(start) )
	#创建socket
	CreatTcpConntion()
	#Get 4 bite data len
	logging.info("befor send ")
	logging.info(fd)
	logging.info(os.getpid())
	ret = fd.sendall(('%c%c' %(chr(int(len(data)/256)),chr(int(len(data)%256))) ).encode('iso-8859-15') + data )
	logging.info("after send")
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
	end = time.time()
	logging.info(" pid = [%d],sendtime = [%.6f],spend = [%.6f] " %(os.getpid(),start,end-start) )
	logging.info('recv data = [%s]' % (recvData ))
	fd.close()
	#return recvData
	#logging.info(bcdhexToaschex(recvData[int(len(myConf.packageHeader)/2):]))
	return bytes(bcdhexToaschex(recvData[int(len(myConf.packageHeader)/2):] ).encode())
def bcdhexToaschex(bcdHex ):
	ascHex =  ''.join(map(lambda x : '%02X' % ord(chr(x)),list(bcdHex))) 
	#logging.info('[backData = [%s]]' % ascHex)
	return ascHex

def triggerTheTrans(transName = '机构签到'):
	logging.info('now starting to trans [%s]' % transName)
	package = pack8583.packPackage8583()
	backData = SendData(package)
	logging.info(backData)
	if backData == None:
		logging.info('no return data')
		return 0
	pack8583.unpack8583(backData)
	pass


def SendOneTrans(transName):
	logging.info(" Processstart = [%.6f] " %(time.time()) )
	triggerTheTrans(transName)
	pass


if __name__ == "__main__":
	transName = "机构签到"
	#先将配置文件加载到内存,提高速度
	ret = myConf.loadCfg(transName)
	logging.info(myConf.packDef)
	#创建进程池
	pool = multiprocessing.Pool(processes=200)
	for i in range(50):
		pool.apply_async(SendOneTrans,(transName,) )
	pool.close()
	pool.join()

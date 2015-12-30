#!/usr/bin/python3.4
#-*- coding:utf8 -*-
from ctypes import *
import logging
import os
import re
import socket
import binascii
import configparser
import myConf
import sys
import time

#packageHeader = b'6000060000602200000000'
libtest = cdll.LoadLibrary(myConf.GetLibName())

#创建TCP链接函数
#功能:创建socket 并connect
def CreatTcpConntion():
	global  fd 
	#创建socket
	fd = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
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
	ret = CreatTcpConntion()
	if ret < 0 :
		logging.error('connect error ! ip = [%s],port = [%d]' % (myConf.ServerIp,myConf.ServerPort))
		return None
	#Get 4 bite data len
	fd.sendall(('%c%c' %(chr(int(len(data)/256)),chr(int(len(data)%256))) ).encode() )
	#send data
	ret = fd.sendall(data)
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
		logging.error('rev data error:' ,e)
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
		logging.error('rev data error:',e)
		fd.close()
		return None
	'''
	offset = len(myConf.packageHeader)/2
	logging.info('offset = %d type of offset = [%s]' % (offset,type(offset)))
	logging.info('bakData = %s len = %d ' % (recvData[int(offset):],len(recvData) ))
	'''
	logging.info('recv data = [%s]' % (recvData ))
	fd.close()
	#return recvData
	#logging.info(recvData[int(len(myConf.packageHeader)/2):])
	return bytes(bcdhexToaschex(recvData[int(len(myConf.packageHeader)/2):] ).encode())

def bcdhexToaschex(bcdHex ):
	ascHex =  ''.join(map(lambda x : '%02X' % ord(chr(x)),list(bcdHex))) 
	#logging.info('[backData = [%s]]' % ascHex)
	return ascHex

def setPackageFlf(n = 0,data='' ):
	if (not isinstance(n,int)) and (not isinstance(data,str)):
		logging.error('error parameter')
		return 0 
	tmpStr = create_string_buffer(1024)
	memset(tmpStr,0,sizeof(tmpStr))
	tmpStr.value = bytes(data.encode()) 
	libtest.packageSet(n,tmpStr)
	pass


def packPackage8583():
	Len = 0
	packageDef = create_string_buffer(bytes(myConf.term8583.encode()),128)
	packageDir = create_string_buffer(bytes(myConf.packageDir.encode()),128)
	ret = libtest.packageInit(packageDir,packageDef )
	if ret < 0:
		logging.error('8583 package init error')
		return None 
	tmpStr = create_string_buffer(1024*2)
	tmp = create_string_buffer(1024)
	memset(tmpStr,0,sizeof(tmpStr))

	setPackageFlf(0,'0800')
	setPackageFlf(11,'000001')
	setPackageFlf(41,'00000001')
	setPackageFlf(42,'000000000000001')
	setPackageFlf(60,'00000025031')
	setPackageFlf(63,'80')

	logging.info('pack ok')
	for i in range(0,128):
	#	logging.info('i = [%d] typeof(i) = [%s]' % (i,type(i)))
		Len = libtest.getFldValue(i,tmp,sizeof(tmp))
		if Len <= 0:
			continue
		logging.info('[%04d][%04d][%s]' % (i, i,tmp.value))
	Len = libtest.packageFinal(tmpStr);	
	logging.info('after pack = [%s]' % tmpStr.value)
	bcd= binascii.a2b_hex(bytes(myConf.packageHeader.encode())) +  binascii.a2b_hex(tmpStr.value)

	return bcd
	pass

def unpack8583(backData):
	Len = 0
	backPackage = create_string_buffer(1024*2)
	backPackage.value = backData
	logging.info('befor unpack = [%s],len = [%d]' %  (backPackage.value,len(backPackage.value ) ) )
	logging.info('len = [%d]' %  (len(backPackage.value ) ) )
	#print('backpackage len = %d' % len(backPackage.value))
	length = c_int(len(backPackage.value))
	#logging.info('backPackage.value = [%s]' % backPackage.value)
	packageDef = create_string_buffer(bytes(myConf.term8583.encode()),128)
	packageDir = create_string_buffer(bytes(myConf.packageDir.encode()),128)
	ret = libtest.packageInit(packageDir,packageDef )
	if ret < 0:
		logging.error('8583 package init error')
	#libtest.unpack8583(packageDir,backPackage,len(backPackage.value))
	libtest.unpack8583(packageDir,backPackage,length.value)
	logging.info('unpack ok')
	tmp = create_string_buffer(1024)
	for i in range(0,128):
		Len = libtest.getFldValue(i,tmp,sizeof(tmp))
		if Len <= 0:
			continue
		logging.info('[%04d][%04d][%s]' % (i, i,tmp.value))
	return libtest.unpackFinal()
	pass

if __name__ == '__main__':
	#package =  pack8583()
	logging.info('---------------------------------------start-------------------------------------------------')
	package =  packPackage8583()
	backData = SendData(package)
	unpack8583(backData)
	#logging.info('backData = [%s]' % backData)
	logging.info('---------------------------------------end---------------------------------------------------')
	
	pass

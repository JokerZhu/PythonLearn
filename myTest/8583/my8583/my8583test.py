#!/usr/bin/python3.4
from ctypes import *
import os
import re
import socket
import logging
import binascii


ServerIP = '192.168.1.90'
ServerPORT = 8383

packageHeader = b'6000060000602200000000'
libtest = cdll.LoadLibrary(os.getcwd() + '/lib8583.so')


logging.basicConfig(level=logging.DEBUG,                                                                                                                                
    format='%(asctime)s [%(filename)s][line:%(lineno)d] [%(process)d] [%(levelname)s] %(message)s',
       datefmt='[%Y%m%d%H%M%S]',
                filename='./log/test.log' ,
                filemode='a+')


#创建TCP链接函数
#功能:创建socket 并connect
def CreatTcpConntion():
	global  fd 
	TimeOut = 10 
	#创建socket
	fd = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	fd.settimeout(TimeOut)
	#connect对端
	try:
		fd.connect((ServerIP,ServerPORT))
	except socket.gaierror as e:
		logging.error('1 can\'t connect your ip/port')
		return -1
	except socket.error as e:
		logging.error('2 can\'t connect your ip/port')
		return -1
	return 0


def SendData(data): 
	ret = CreatTcpConntion()
	if ret < 0 :
		logging.error('connect error ! ip = [%s],port = [%d]' % (ServerIP,ServerPORT))
		return None
	#Get 4 bite data len
	fd.sendall(('%c%c' %(chr(int(len(data)/256)),chr(int(len(data)%256))) ).encode() )   
	#send data
	ret = fd.sendall(data)
	if ret != None:
		logging.error('send error!')
		fd.close()
		return None 
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
	#rev Data
	try:
		recvData = fd.recv(recvDataLen[0]* 256 + recvDataLen[1] )
	except socket.timeout as e:
		logging.error('rev data error:',e)
		fd.close()
		return None
	'''
	offset = len(packageHeader)/2
	logging.info('offset = %d type of offset = [%s]' % (offset,type(offset)))
	logging.info('bakData = %s len = %d ' % (recvData[int(offset):],len(recvData) ))
	'''
	fd.close()
	#return recvData
	logging.info(recvData[int(len(packageHeader)/2):])
	return bytes(bcdhexToaschex(recvData[int(len(packageHeader)/2):] ).encode())

def bcdhexToaschex(bcdHex ):
	ascHex =  ''.join(map(lambda x : '%02X' % ord(chr(x)),list(bcdHex))) 
	logging.info(ascHex)
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
	ret = libtest.packageInit()
	if ret < 0:
		logging.error('8583 package init error')
		return None 
	tmpStr = create_string_buffer(1024*2)
	memset(tmpStr,0,sizeof(tmpStr))

	setPackageFlf(0,'0800')
	setPackageFlf(11,'000001')
	setPackageFlf(41,'00000001')
	setPackageFlf(42,'000000000000001')
	setPackageFlf(60,'00000025031')
	setPackageFlf(63,'80')

	len = libtest.packageFinal(tmpStr);	
	logging.info('result = [%s]' % tmpStr.value)
	bcd= binascii.a2b_hex(bytes(packageHeader)) +  binascii.a2b_hex(tmpStr.value)
	return bcd
	pass



if __name__ == '__main__':
	#package =  pack8583()
	package =  packPackage8583()
	backData = SendData(package)
	logging.info('backData = [%s]' % backData)
	backPackage = create_string_buffer(1024*2)
	backPackage.value = backData
	'''
	hex =  bytes(bcdhexToaschex(backData).encode())
	backPackage.value = hex 
	'''
	logging.info('backPackage.value = [%s]' % backPackage.value)
	ret = libtest.packageInit()
	if ret < 0:
		logging.error('8583 package init error')
	libtest.unpack8583(backPackage,len(backPackage.value))
	
	

	pass

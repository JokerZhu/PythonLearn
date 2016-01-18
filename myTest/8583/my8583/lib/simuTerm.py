#!/usr/bin/python3.4
#-*- coding:utf8 -*-
from ctypes import *
import logging
import os
import re
import socket
import binascii
import configparser
import customizeFun
import myConf
import sys
import time
import Security
import pack8583

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
	logging.info('recv data = [%s]' % (recvData ))
	fd.close()
	#return recvData
	#logging.info(recvData[int(len(myConf.packageHeader)/2):])
	return bytes(bcdhexToaschex(recvData[int(len(myConf.packageHeader)/2):] ).encode())

def bcdhexToaschex(bcdHex ):
	ascHex =  ''.join(map(lambda x : '%02X' % ord(chr(x)),list(bcdHex))) 
	#logging.info('[backData = [%s]]' % ascHex)
	return ascHex



def triggerTheTrans(transName = '签到'):
	logging.info('now starting to trans [%s]' % transName)
	package = pack8583.packPackage8583(transName )
	backData = SendData(package)
	if backData == None:
		logging.info('no return data')
		return 0
	pack8583.unpack8583(backData)
	pass

if __name__ == '__main__':
	#package =  pack8583()
	logging.info('---------------------------------------start-------------------------------------------------')
	#package =  packPackage8583()
	#backData = SendData(package)
	#unpack8583(backData)
	#unpack8583(bytes('0200302004C120C09811000000000000000100000501021000120800000001356227001823260036733D00000000000000003737373738383838313030303030303030303030303031313536533A4D1CA0F27BDC26000000000000000014220000010005013933313036384132'.encode()))
	#unpack8583(bytes('0200202004C120C09811310000000500021000120800000001356227001823260036733D00000000000000003737373738383838313030303030303030303030303031313536533A4D1CA0F27BDC26000000000000000014010000010005003437303636333734'.encode()))
	#logging.info('backData = [%s]' % backData)
	while(True):
		os.system('clear')
		print('-------------------starting--------------------')
		print('please input what you want to do(0 for exit)')
		#读到已注册的交易类型
		transTypeList = myConf.ReadAllTransType()
		logging.info(transTypeList)
		for index in range(len(transTypeList)):
			print('%d: %s ' % (index+1,transTypeList[index]) )
		try:
			choice = int(input('your choice:'))
			if choice == 0:
				break
			transTypeChoice = transTypeList[choice - 1]
			print('now you chioce :%s ' % transTypeChoice)
		except ValueError as e:
			print('your input is error,try again')
			continue
		except IndexError as e:
			print('your input is error,try again')
			continue
		triggerTheTrans(transTypeChoice)
		continue




	logging.info('---------------------------------------end---------------------------------------------------')
	
	pass

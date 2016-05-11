#!/usr/local/bin/python3.4
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

currentSeq = 1
currentTimes = 0

#压力测试标识
isStressTest = 0
#进度条显示函数
#功能:显示一个进度条
#   now:当前进度
#   sum:总数
#   返回:无
def ViewBar(now = 1,sum = 100):
	barLen = 100
	hashtag = '#' * int(now / sum * barLen)
	space  = ' ' * int(barLen - (int(now / sum * barLen) ))                                                                                                            
	sys.stdout.write('\r[%s] %.2f%%   %d/%d ' % (hashtag + space, (now/sum) * 100,now,sum) )
	pass

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
	global currentTimes
	status = ''
	recvData = ''
	start = time.time()
	logging.info(" starttime = [%.6f] " %(start) )
	#创建socket
	if CreatTcpConntion() < 0:
		return None 

	#Get 4 bite data len
	logging.info("befor send ")
	#logging.info(fd)
	#logging.info(os.getpid())
	try:
		ret = fd.sendall(('%c%c' %(chr(int(len(data)/256)),chr(int(len(data)%256))) ).encode('iso-8859-15') + data )
	except BrokenPipeError as e:
		logging.error('send error [%s]!' % e)
		fd.close()
		return None 
	#链接成功计算时间
	#Now = time.time()
	#logging.info("starttime =[%.6f] ConnectTime = [%.6f]" % (start,Now - start))
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
		status = 'TO'
	#rev Data
	if status != 'TO':
		if int(recvDataLen[0]* 256 + recvDataLen[1]) <= 0:
			logging.error('rev data len error')
		else:
			logging.info('recv data  len OK len = [%d] !' % (recvDataLen[0]* 256 + recvDataLen[1]))
		try:
			recvData = fd.recv(recvDataLen[0]* 256 + recvDataLen[1] )
		except socket.timeout as e:
			logging.error('rev data error: %s ' % e)
			status = 'TO'
		status = 'OK'

	end = time.time()
	#logging.info("times= [%d]status=[%s] sendtime = [%s.%.5s] spend=[%.6f] seq=[%06d]" %(\
	#	currentTimes,status,time.strftime("%X", time.localtime(start )),str(start).split('.')[1],end-start,customizeFun.currentSeq))
	logging.info("current seq = [%d]" % customizeFun.currentSeq)
	logging.info('recv data = [%s]' % (recvData ))
	if fd:
		fd.close()
	#return recvData
	#logging.info(bcdhexToaschex(recvData[int(len(myConf.packageHeader)/2):]))
	return bytes(customizeFun.bcdhexToaschex(recvData[int(len(myConf.packageHeader)/2):] ).encode()),status,start,end


#根据交易名发送交易
def triggerTheTrans(transName = '机构签到'):
	global isStressTest	
	global currentTimes
	respcode = ''
	logging.info('now starting to trans [%s] [%d]' % (transName,isStressTest))
	if isStressTest == 0:
		#获取当前流水
		seq = myConf.GetSerialNo()
		#保存最大流水号(先保存)
		currentSeq = seq + 1 
		customizeFun.currentSeq = currentSeq
		currentTimes = 0
		myConf.setSeqToSeqfile(currentSeq)
		package = pack8583.packPackage8583(transName )
	else:
		tmp = pack8583.packPackage8583ForStress(transName)
		logging.info(tmp)
		package = binascii.a2b_hex(bytes((myConf.packageHeader + tmp).encode("'iso-8859-15'" ))) 

	logging.info(package)
	backData,status,start,end = SendData(package)
	logging.info(backData)
	if backData == None or len(backData) ==0 :
		logging.info('no return data')
	else:
		ret,respcode = pack8583.unpack8583(backData)
	logging.info("times= [%d]status=[%s] sendtime = [%s.%.5s] spend=[%.6f] seq=[%06d],respcode=[%s]" %(\
		currentTimes,status,time.strftime("%X", time.localtime(start )),str(start).split('.')[1],end-start,customizeFun.currentSeq,respcode))
	pass

#一笔交易
def SendOneTrans(transName,i,j):
	#当前第几轮
	global currentTimes
	logging.info(" Processstart = [%.6f] " %(time.time()) )
	#logging.info(" i = [%d] " %(i) )
	#设置当前交易的流水号
	if i > 999999:
		i = i % 999999
	customizeFun.currentSeq = i

	logging.info(" i = [%d] " %(customizeFun.currentSeq ) )
	currentTimes = j
	triggerTheTrans(transName)
	pass
#######################################
#
#	压力测试
#######################################
def stressTest():
	global isStressTest	
	isStressTest = 1
	count = 5
	SUM = 100
	MAXprocesses = 500
	currentSeq = 0
	transName = "消费"

	currentCount = 0
	currentSum = 0
	currentProce = 0
	#获取当前流水
	seq = myConf.GetSerialNo()
	#测试参数
	currentCount = 50000
	currentSum = 5
	'''
	while(True):
		print('请输入你想要做多少轮(当前默认值为:%d),\'0\' for exit' % count)
		choice = input('your input:')
		if len(choice) == 0:
			currentCount = count
			break
		else:
			try:
				currentCount = int(choice)
				if currentCount == 0:
					return 0
			except ValueError as e:
				print('your input is error,must be number,try again')
				continue
			break

	while(True):
		print('请输入你想要每轮多少笔交易(当前默认值为:%d),\'0\' for exit' % SUM)
		choice = input('your input:')
		if len(choice) == 0:
			currentSum = SUM
			break
		else:
			try:
				currentSum = int(choice)
				if currentSum == 0:
					return 0
			except ValueError as e:
				print('your input is error,must be number,try again')
				continue
			break
	'''
	#保存最大流水号(先保存)
	currentSeq = seq + currentCount * currentSum
	myConf.setSeqToSeqfile(currentSeq)

	currentProce = currentSum * myConf.TimeOut
	if currentProce > MAXprocesses:
		currentProce = MAXprocesses
	#先清空现有日志
	myConf.cleanCurrentLog()

	#创建进程池
	pool = multiprocessing.Pool(processes=currentProce)
	for j in range(currentCount):
		for i in range(currentSum):
			#开始启动进程
			pool.apply_async(SendOneTrans,(transName,seq + currentSum * j + i, j ) )
		ViewBar(j+1,currentCount)
		if j+1 == currentCount:
			print('\n发送完成，等待返回...')
		time.sleep(1)
	pool.close()
	pool.join()
	#生成结果文件
	CMD = "grep spend %s |grep -v grep> ../log/%06d " % (myConf.GetLogFileName(),currentSeq )
	logging.info(CMD)
	os.system(CMD)

if __name__ == "__main__":
	#先将配置文件加载到内存,提高速度
    #加载交易返回定义文件
	myConf.loadAllTransTypecfg()
	#初始化8583定义
	stressTestNo = 0
	stressTest()
	'''
	while(True):
		os.system('clear')
		print('-------------------starting--------------------')
		print('please input what you want to do(0 for exit)')
		#读到已注册的交易类型
		transTypeList = myConf.ReadAllTransType()
		stressTestNo = len(transTypeList) +1
		#logging.info(transTypeList)
		for index in range(len(transTypeList)):
			print('%d: %s ' % (index+1,transTypeList[index]) )
		print('%d: %s ' % ( stressTestNo ,"压力测试" ))
		try:
			choice = int(input('your choice:'))
			if choice == 0:
				break
			elif choice == stressTestNo:
				stressTest()
				continue
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
	'''

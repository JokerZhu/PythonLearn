#!/usr/bin/python3.4
#-*- coding:utf-8 -*-
import os
import sys
import time
import socket


#HSM的IP地址
IP = '211.147.236.34'
#HSM的端口
PORT = 10002 
#源SEK
SEKSOURCE = '0024'
#目的SEK
SEKDEST = '0007'
#中间TEK
TEK = '0001'


#源文件名
SOURCEFILE = 'source.txt'
TMPFILE = 'tmp.txt'
DESTFILE = 'dest.txt'

#创建TCP链接函数
#功能:创建socket 并connect加密机
def CreatTcpConntion():
	global  fd 
	#创建socket
	fd = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	#connect对端
	fd.connect((IP,PORT))

#加密机数据收发函数
#功能:将发送给加密机的数据加上消息长度并发送
#	data:需要发送给加密机的数据
#	返回:从加密机返回来的数据
def SendData(data): 
	'''
	global  fd 
	#创建socket
	fd = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	#connect对端
	fd.connect((IP,PORT))
	'''
	#首先发两个字节的长度
	fd.sendall(('%c%c' %(chr(int(len(data)/256)),chr(int(len(data)%256))) ).encode() )
	#再发送数据
	fd.sendall(data.encode())
	#接收两个字节的长度
	recvDataLen = fd.recv(2)
	#接收数据
	recvData = fd.recv(recvDataLen[0]* 256 + recvDataLen[1] )
#	fd.close()
	return recvData



#加密机KE命令
#功能:拼装加密机KE指令，并发送到加密机
#	type(0/1): 0从SEK到TEK   1从TEK到SEK
#	SEK:加密机中的SEK密钥索引
#	TEK:加密机中TEK的密钥索引
#	返回：list[0] 为'00'即为成功
def HsmCmdKE(type,SEK,TEK,inputKey):
	typeList = [0,1]
	#参数检查
	if type not in typeList:
		print('type err %d!!  should(0/1) ' % type )
		return ['-1','type err']
	if SEK =='' or TEK == '' or inputKey == '':
#		print('parameters error !')
		return ['-1','input key empty']
	#参数检查
	if(len(inputKey) == 16):
		keyType = 'X'
	elif(len(inputKey) == 32):
		keyType = 'Y'
	elif(len(inputKey) == 64):
		keyType = 'Z'
	else:
#		print('parameters error !')
		return ['-2','key len = %s error' % len(inputKey)]
	#拼接命令
	CMD = ('KES%sT%s%d%s%s' % (SEK,TEK,type,keyType,inputKey ))
#	print('CMD = ',CMD)
	#收发数据
	result = SendData(CMD).decode()
#	print('result = %s' % result)
	returnCode = result[2:4]
	if returnCode != '00':
		return [returnCode,'']
	else:
		return [returnCode,result[4:len(result) - 16]]


#转密钥函数
#功能:将源SEK加密的密钥转成目标SEK加密的密钥
#	sourceKey:使用源SEK加密的密钥
#	返回:返回一个list list[0]为加密机返回码+自定义返回码 list[1]为错误原因或者转出来的结果 
def ExchangeKey(sourceKey):
	#1、调KE命令先将密钥转成使用TEK加密
	result = HsmCmdKE(0,SEKSOURCE,TEK,sourceKey)
	if('00' != result[0]):
		return result
	#2、再调KE命令将密钥转成使用目的SEK加密
	result = HsmCmdKE(1,SEKSOURCE,TEK,result[1])
	return result

#进度条显示函数
#功能:显示一个进度条
#	now:当前进度
#	sum:总数
#	返回:无
def ViewBar(now = 1,sum = 100):
	barLen = 100
	hashtag = '#' * int(now / sum * barLen)
	space  = ' ' * int(barLen - (int(now / sum * barLen) )) 
	sys.stdout.write('\r[%s] %.2f%%  the [%d] end' % (hashtag + space, (now/sum) * 100,now) )
	pass

#密钥转换函数
#功能:将从文件中读出数据，再进行转密钥，并把结果写入目标文件
#返回: 0为成功，其他为失败

def StartExchangeKeys():
	i = 0
	#循环从文件中读取一行数据，放到一个list里
	try:
		with open(SOURCEFILE,'r') as fileIn ,open(TMPFILE,'w') as fileOut:
			try:
				l =  fileIn.readlines()
				sum = len(l)
				print('sum of need to exchange keys: ',sum)
				for line in l :
					i += 1
					line = line.strip('\n')
					lines = line.split(',')
					try:
						lines[4]
						result = ExchangeKey(lines[4])
						lines += result
					except IndexError:
					#	print('key is empty') 
						pass
					#print(lines)
					fileOut.writelines(['%s|' % item for item in lines])
					fileOut.writelines('\n' )
					ViewBar(i,sum)
			finally:
				fileIn.close()
	except FileNotFoundError:
		print('here is no file named [%s]' % SOURCEFILE)
		return -1
	#排序:将错误的排到文件尾
	'''
	try:
		with open(TMPFILE,'r') as fileTmp,open(DESTFILE,'w') as fileDest:
	except FileNotFoundError:
		print('here is no file named [%s] or [%s]' %(TMPFILE,DESTFILE) )
		return -1
	'''
	

	print ('\n')
	return 0

def sedDataToHsm():
	while(1):
		data = input('please in put you cmd(\'exit\' for exit) :')
		if data == 'exit':
			return 0
		else:
			returnData = SendData(data)
			print('return code : %s return message : %s ' % (returnData[2:4].decode(), returnData[4:len(returnData)].decode()))
	pass
def changeHsmInfo():
	print('in changeHsmInfo')
	pass
if __name__ == '__main__':
	switch = {1:StartExchangeKeys,2:sedDataToHsm,3:changeHsmInfo}

	os.system('clear')
	print('string.....')
	CreatTcpConntion()
	print('HSM connect succes.\n\ncurrent HSM IP = [%s] PORT = [%s] ' % (IP,PORT))
	while(1):
		print('%s start %s' % ('=' * 60,'=' * 60) )
		print('\tplease input what you want to do:')
		print('\t0、exit.')
		print('\t1、start exchange term masterkey of POSP.')
		print('\t2、send CMD to HSM server.')
		print('\t3、change HSM ip and port.')
		
		try:
			inputCmd = int(input('my choice:'))
			if 0 == inputCmd:
				break
		except ValueError:
			print('input error,please reinput!!')
			continue
			
		print('you chose :',inputCmd)
		try:
			switch[inputCmd]()
		except KeyError:
			print('input error,please reinput!!')
			continue
	
	
	'''
	if StartExchangeKeys() < 0:
		print('StartExchangeKeys error ')
		fd.close()
		return 0
	'''
	fd.close()

	pass

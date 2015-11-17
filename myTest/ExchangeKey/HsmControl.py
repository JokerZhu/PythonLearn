#!/usr/bin/python3.4
#-*- coding:utf-8 -*-
import os
import sys
import time
import socket
import ipaddress
import MySQLdb


#HSM的IP地址
#IP = 'XXX.XXX.XXX.XXX'
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

errorMap = {'00':'正确', 
'01':'无主密钥', 
'02':'无工作密钥1', 
'03':'无工作密钥2', 
'04':'工作密钥1奇偶校验错', 
'05':'工作密钥2奇偶校验错', 
'06':'无老的主密钥', 
'10':'口令错', 
'11':'密码机不在授权状态', 
'12':'没有插IC卡(从串行口进入密钥管理时要插A卡)', 
'13':'写IC卡错', 
'14':'读IC卡错', 
'15':'IC卡不配套', 
'16':'打印机没准备好', 
'17':'IC卡未格式化', 
'18':'打印格式没定义', 
'20':'MAC校验错', 
'21':'MAC标志指示域错', 
'22':'密钥长度与使用模式不符', 
'23':'MAC模式指示域错', 
'24':'数据长度指示域错（不为8的倍数、大于8192字节）', 
'26':'加密模式指示域错', 
'27':'加解密标志错', 
'28':'PIN格式错', 
'29':'PIN检查长度大于实际PIN长度', 
'31':'工作密钥1标志错', 
'32':'工作密钥2标志错', 
'33':'工作密钥索引错', 
'34':'密钥离散次数错', 
'35':'PIN参考值校验错', 
'36':'主帐号参考值校验错', 
'37':'PIN校验错', 
'38':'PIN长度错（小于4或者大于12）', 
'39':'CVN标志错', 
'40':'DES算法模块错误', 
'41':'SSF33算法模块错误', 
'60':'无此命令', 
'61':'消息太短', 
'62':'消息太长', 
'63':'消息检查值错', 
'76':'子网掩码无效', 
'77':'非法字符', 
'78':'文件尾', 
'79':'客户IP地址语法错'} 

#创建TCP链接函数
#功能:创建socket 并connect加密机
def CreatTcpConntion():
	global  fd 
	#创建socket
	fd = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	#connect对端
	try:
		fd.connect((IP,PORT))
	except ConnectionRefusedError:
#		print('can\'t connect your ip/port')
		return -1
	return 0

def ReCreatTcpConntion():
	fd.close()
	print('reconntioning....ip =[%s] port = [%d]' % (IP,PORT))
	return  CreatTcpConntion()
	pass
#加密机数据收发函数
#功能:将发送给加密机的数据加上消息长度并发送
#	data:需要发送给加密机的数据
#	返回:从加密机返回来的数据
def SendData(data): 
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
		return ['-1','type err','']
	if SEK =='' or TEK == '' or inputKey == '':
#		print('parameters error !')
		return ['-1','input key empty','']
	#参数检查
	if(len(inputKey) == 16):
		keyType = 'X'
	elif(len(inputKey) == 32):
		keyType = 'Y'
	elif(len(inputKey) == 64):
		keyType = 'Z'
	else:
#		print('parameters error !')
		return ['-2','key len = %s error' % len(inputKey),'']
	#拼接命令
	CMD = ('KES%sT%s%d%s%s' % (SEK,TEK,type,keyType,inputKey ))
#	print('CMD = ',CMD)
	#收发数据
	result = SendData(CMD).decode()
#	print('result = %s' % result)
	returnCode = result[2:4]
	if returnCode != '00':
		return [returnCode,errorMap[returnCode],'']
	else:
		return [returnCode,errorMap[returnCode],result[4:len(result) - 16]]


#转密钥函数
#功能:将源SEK加密的密钥转成目标SEK加密的密钥
#	sourceKey:使用源SEK加密的密钥
#	type:1:old到new 2:new 到old
#	返回:返回一个list list[0]为加密机返回码+自定义返回码 list[1]为错误原因或者转出来的结果 
#	
def ExchangeKey(sourceKey,type = 1 ):
	if 1 == type:
		#1、调KE命令先将密钥转成使用TEK加密
		result = HsmCmdKE(0,SEKSOURCE,TEK,sourceKey)
		if('00' != result[0]):
			return result
		#2、再调KE命令将密钥转成使用目的SEK加密
		result = HsmCmdKE(1,SEKDEST,TEK,result[2])
		return result
	else:
		#1、调KE命令先将密钥转成使用TEK加密
		result = HsmCmdKE(0,SEKDEST,TEK,sourceKey)
		if('00' != result[0]):
			return result
		#2、再调KE命令将密钥转成使用目的SEK加密
		result = HsmCmdKE(1,SEKSOURCE,TEK,result[2])
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
	sys.stdout.write('\r[%s] %.2f%%   %d/%d ' % (hashtag + space, (now/sum) * 100,now,sum) )
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
#				print('sum of need to exchange keys: ',sum)
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
	CMD = 'grep  "|00|" %s > %s ' % (TMPFILE,DESTFILE)
	os.system(CMD)
	CMD = 'grep  -v "|00|" %s >> %s ' % (TMPFILE,DESTFILE)
	os.system(CMD)
#	os.sys('grep  -v "|00|" %s >> %s ' % (TMPFILE,DESTFILE))
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
	global IP
	global PORT

	ipBak = IP
	portBak = PORT

	ip = input('please input your ip:')
	port = input('please input your port:')
	try:
		ipaddress.ip_address(ip)
		IP = ip
	except ValueError:
		print('input ip error')
		return -1
	
	try:
		PORT = int(port)
	except ValueError :
		print('input port error')
		return -1

	
	if ReCreatTcpConntion() < 0:
		print('can\'t connect ip [%s] port [%d]' % (IP,PORT) )
		print('connecting defult ip [%s] port [%d]' % (ipBak,portBak))
		IP = ipBak
		PORT = portBak
		return ReCreatTcpConntion()
	else:
		pass

	pass

def oldToNew():
	while(1):
		inputData = input('please input your key(exit to exit):')
		if inputData == 'exit':
			return 0
		result = ExchangeKey(inputData)
		print('code = [%s],msg = [%s],key = [%s].' % (result[0],result[1],result[2]))
	
	pass
def newToOld():
	while(1):
		inputData = input('please input your key(exit to exit):')
		if inputData == 'exit':
			return 0
		result = ExchangeKey(inputData,2)
		print('code = [%s],msg = [%s],key = [%s].' % (result[0],result[1],result[2]))
	pass

if __name__ == '__main__':
	switch = {1:StartExchangeKeys,2:sedDataToHsm,3:changeHsmInfo,4:oldToNew,5:newToOld}

	global IP
	global PORT

	os.system('clear')
	print('string.....')
	CreatTcpConntion()
	while(1):
		print('\n\ncurrent HSM IP = [%s] PORT = [%d] ' % (IP,PORT))
		print('%s start %s' % ('=' * 60,'=' * 60) )
		print('\tplease input what you want to do:')
		print('\t0、exit.')
		print('\t1、start exchange term masterkey of POSP.')
		print('\t2、send CMD to HSM server.')
		print('\t3、change HSM ip and port.')
		print('\t4、old to new.')
		print('\t5、new to old.')
		
		try:
			inputCmd = int(input('my choice:'))
			if 0 == inputCmd:
				break
		except ValueError:
			print('input error,please reinput!!')
			continue
			
		print('you chose :',inputCmd)
		try:
			ret = switch[inputCmd]()
		except KeyError:
			print('input error,please reinput!!')
			continue
		print('ret = ',ret)
		continue
	
	fd.close()

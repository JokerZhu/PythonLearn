#!/usr/bin/python3.4
#-*- coding:utf-8 -*-
import os
import sys
import time
import socket
import ipaddress
import binascii
import MySQLdb
import logging
import myConf


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
def CreatHsm():
	global  fd 
	#创建socket
	fd = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	fd.settimeout(10)
	#connect对端
	try:
		fd.connect((IP,PORT))
	except ConnectionRefusedError:
		print('can\'t connect your ip/port')
		return -1
	except socket.timeout as e:
		print('can\'t connect your ip/port')
		return -1
		fd.close()
	return 0


#功能:于加密机断开连接
def disConnHsm():
	try:
		fd.close()
	except ConnectionRefusedError:
		return -1
	return 0




def ReCreatTcpConntion():
	fd.close()
	print('reconntioning....ip =[%s] port = [%d]' % (IP,PORT))
	return CreatHsm()
	pass
#加密机数据收发函数
#功能:将发送给加密机的数据加上消息长度并发送
#	data:需要发送给加密机的数据
#	返回:从加密机返回来的数据
def SendData(data,hex = ''): 
	logging.info('hsmCmd = [%s]' % data )
	#首先发两个字节的长度
	fd.sendall(('%c%c' %(chr(int((len(data) + len(hex))/256)),chr(int((len(data) + len(hex))%256))) ).encode() )
	#再发送数据
	fd.sendall(data.encode())
	if len(hex) > 0:
		logging.info(hex)
		if type(hex) == bytes:
			fd.sendall(hex)
		else:
			fd.sendall(hex.encode())
	#接收两个字节的长度
	recvDataLen = fd.recv(2)
	#接收数据
	recvData = fd.recv(recvDataLen[0]* 256 + recvDataLen[1] )
	logging.info(recvData)
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
	elif(len(inputKey) == 48):
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

#加密机K2命令
#功能:拼装加密机K2指令，并发送到加密机
# SEK1 : 密钥索引1
# SEK2 : 密钥索引2
# TMK  : 使用SEK1加密的TMK密文
# TYPE : PIK/MAK密钥长度(X：64比特密文 Y：128 比特密文 Z：192 比特密文)

def HsmCmdK2(SEK1,SEK2,TMK,TYPE):
	typeList = ['X','Y','Z']
	#参数检查
	if TYPE not in typeList:
		print('type err %s!!  should(X/Y/Z) : ' % TYPE)
		return ['-1','type err','']
	if SEK1=='' or SEK2 == '' or TMK == '':
#		print('parameters error !')
		return ['-1','input key empty','']
	#参数检查
	if(len(TMK) == 16):
		keyType = 'X'
	elif(len(TMK) == 32):
		keyType = 'Y'
	elif(len(TMK) == 48):
		keyType = 'Z'
	else:
#		print('parameters error !')
		return ['-2','key len = %s error' % len(inputKey),'']
	#拼接命令
	CMD = ('K2S%sS%s%s%s%s' % (SEK1,SEK2,keyType,TMK,TYPE ))
#	print('CMD = ',CMD)
	#收发数据
	result = SendData(CMD).decode()
#	print('result = %s' % result)
	returnCode = result[2:4]
	if returnCode != '00':
		return [returnCode,errorMap[returnCode],'']
	else:
		return [returnCode,errorMap[returnCode],result[4:len(result)]]

#生成终端工作密钥:PINKEY

def GenTermPinKey(TMK,SEK = ''):
	#myConf.SEK
	lenList = [16,32,48]
	if len(TMK) not in lenList:
		logging.error("input TMK is ERROR")	
		return None
	TMKLEN = len(TMK)
	#参数检查
	if TMKLEN == 16:
		keyType = 'X'
	elif TMKLEN == 32:
		keyType = 'Y'
	elif TMKLEN == 48:
		keyType = 'Z'
	if SEK :
		result = HsmCmdK2(SEK,SEK,TMK,keyType)
	else:
		result = HsmCmdK2(myConf.SEK,myConf.SEK,TMK,keyType)
	print(result[2])
	if result[0] == '00':
		#return result[5:5 + TMKLEN], result[5 + TMKLEN,5 + TMKLEN + TMKLEN] ,result[5 + TMKLEN,5 + TMKLEN + TMKLEN: ]
		return result[0], result[2][0:TMKLEN],result[2][TMKLEN:TMKLEN*2],result[2][TMKLEN*2:]
		#return result[2]
	else:
		return result

#生成终端工作密钥:MACKEY

def GenTermMacKey(TMK,SEK = ''):
	#myConf.SEK
	lenList = [16,32,48]
	if len(TMK) not in lenList:
		logging.error("input TMK is ERROR")	
		return None
	TMKLEN = len(TMK)
	#参数检查
	if TMKLEN == 16:
		keyType = 'X'
	elif TMKLEN == 32:
		keyType = 'Y'
	elif TMKLEN == 48:
		keyType = 'Z'

	if SEK :
		result = HsmCmdK2(SEK,SEK,TMK,'X')
	else:
		result = HsmCmdK2(myConf.SEK,myConf.SEK,TMK,'X')
	print(result[2])
	if result[0] == '00':
		#return result[5:5 + TMKLEN], result[5 + TMKLEN,5 + TMKLEN + TMKLEN] ,result[5 + TMKLEN,5 + TMKLEN + TMKLEN: ]
		return result[0],result[2][0:16],result[2][16:16*2],result[2][16*2:]
		#return result[2]
	else:
		return result
	pass

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

def GenMACPOSP(pack = '',SEK = '',mackey= '' ):
	#先异或

	lenList = [16,32,48]
	if len(mackey) not in lenList:
		logging.error("input mackey is ERROR")	
		return None
	TMKLEN = len(mackey)
	#参数检查
	if TMKLEN == 16:
		keyType = 'X'
	elif TMKLEN == 32:
		keyType = 'Y'
	elif TMKLEN == 48:
		keyType = 'Z'
	length = len(pack)
	logging.info('length = %d' % length)
	#CMD = 'M02S%s%s%s' % (SEK,keyType,mackey) + '%04d' %  len(pack) + pack
	CMD = 'M02S%s%s%s%04d' % (SEK,keyType,mackey , len(pack))

	result = SendData(CMD,pack).decode()
	print('result = %s' % result)


	returnCode =result[2:4]
	if returnCode != '00':
		return [returnCode,errorMap[returnCode],'']
	else:
		return [returnCode,errorMap[returnCode],result[4:len(result)]]
	pass



#CreatHsm()
#GenMACPOSP(binascii.a2b_hex('0200302004C020C0981100000000000000005100183402100006324392260009942820D1312101120084393136363838303134303030303030303030303030303031313536ACF46E73838A82F92600000000000000000822000001'),'0018','AD915C199AF5C1EF')
#GenMACPOSP(binascii.a2b_hex('0200202004C020C0981131000082765102100006324392260009942820D13121011200843933333135303032323831323333313534353131303031343135364F2FA2CF1E5500E926000000000000000013010000010005'),'0018','655499F6D6C430C6')
#GenMACPOSP('0200602006C020C08A11166214441000010053310000827656052000050006296214441000010053D30102200000003333313530303232383132333331353435313130303134313536260000000000000001129F2608BBFD1BFACF6A64299F2701809F100807000103A0A002019F3704989636539F36020334950500800088009A031603099C01319F02060000000000005F2A02015682023C009F1A0201569F03060000000000009F3303E0E1C89F34031E03009F3501229F1E083132333435363738001301000001000500','0018','9F9F09222845B244')

#GenMACPOSP('\x02\x00\x30\x20\x04\xC0\x20\xC3\x03\x03\x03\x03\x03\x03\x03\x03\x03\x03\x03\x03\x03\x03\x13\x13\x53\x6A','0018','AD915C199AF5C1EF')
#disConnHsm()
#disConnHsm()

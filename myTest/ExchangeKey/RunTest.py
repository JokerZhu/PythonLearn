#!/usr/bin/python3.4
import os
import time
import socket


#HSM的IP地址
IP = 'XXX.XXX.XXX.XXX'
#HSM的端口
PORT = 10002 
#源SEK
SEKSOURCE = '0018'
#目的SEK
SEKDEST = '0018'
#中间TEK
TEK = '0001'

#源文件名
SOURCEFILE = 'source.txt'
DESTEFILE = 'dest.txt'

#加密机通讯函数
def SendData(data): 
	global  fd 
	#创建socket
	fd = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	#connect对端
	fd.connect((IP,PORT))
	#首先发两个字节的长度
	fd.sendall(('%c%c' %(chr(int(len(data)/256)),chr(int(len(data)%256))) ).encode() )
	#再发送数据
	fd.sendall(data.encode())
	#接收两个字节的长度
	recvDataLen = fd.recv(2)
	#接收数据
	recvData = fd.recv(recvDataLen[0]* 256 + recvDataLen[1] )
	fd.close()
	return recvData



#加密机KE命令
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
		print('parameters error !')
		return ['-1','parameters error']
	#参数检查
	if(len(inputKey) == 16):
		keyType = 'X'
	elif(len(inputKey) == 32):
		keyType = 'Y'
	elif(len(inputKey) == 64):
		keyType = 'Z'
	else:
		print('parameters error !')
		return ['-1','parameters error']
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
def ExchangeKey(sourceKey):
	#1、调KE命令先将密钥转成使用TEK加密
	result = HsmCmdKE(0,SEKSOURCE,TEK,sourceKey)
	if('00' != result[0]):
		return result
	#2、再调KE命令将密钥转成使用目的SEK加密
	result = HsmCmdKE(1,SEKSOURCE,TEK,result[1])
	return result


if __name__ == '__main__':
	#循环从文件中读取一行数据，放到一个list里
	fp = open(SOURCEFILE,'r')
	l =  fp.readlines()
	for i in l :
		lines = i.split()
		try:
			print(lines[4])
		except Exception as e:
			print(e)
	fp.close()
	#如果有上级主密钥，则发到加密机进行转KEY

	#将结果写到目标文件内
#	result = ExchangeKey('8EBB00D03EAD89148EBB00D03EAD8914')
#	print (result)

#!/usr/bin/python3.4
#-*- coding:utf8 -*-
import binascii
import logging
import myConf
from pyDes import * 
import re

#3DES 加密
def tripleDesEncrypt(sourceData='',tripleDesKey = '11111111111111111111111111111111'):
	if(len(sourceData) <= 0 ):
		logging.error('input data len error')
		return None 
	#将数据分成8个字节进行3DES加密，不足8个字节填充0x00补齐8个字节
	try:
		t1 = triple_des(binascii.unhexlify(tripleDesKey),ECB,b'\0\0\0\0\0\0\0\0',pad=None ,padmode=PAD_NORMAL)
		result = t1.encrypt(binascii.unhexlify(sourceData) )
	except ValueError as e:
		logging.error('in tripleDesEncrypt :%s' % e)
		return None
	return binascii.hexlify(result).decode().upper() 

#单DES加密
def DesEncrypt(sourceData='',tripleDesKey = '1111111111111111'):
	if (not isinstance(sourceData,str)) or len(sourceData) <= 0:
		logging.error('input data len error')
		return None 
	try:
		t1 = des(binascii.unhexlify(tripleDesKey),ECB,b'\0\0\0\0\0\0\0\0',pad=None ,padmode=PAD_NORMAL)
		result = t1.encrypt(binascii.unhexlify(sourceData) )
	except ValueError as e:
		logging.error('in tripleDesEncrypt :%s' % e)
		return None
	return binascii.hexlify(result).decode().upper() 

def DesDecrypt(sourceData='',tripleDesKey = '1111111111111111'):
	if (not isinstance(sourceData,str)) or len(sourceData) <= 0:
		logging.error('input data len error')
		return None 
	try:
		t1 = des(binascii.unhexlify(tripleDesKey),ECB,b'\0\0\0\0\0\0\0\0',pad=None ,padmode=PAD_NORMAL)
		result = t1.decrypt(binascii.unhexlify(sourceData) )
	except ValueError as e:
		logging.error('in tripleDesEncrypt :%s' % e)
		return None
	return binascii.hexlify(result).decode().upper() 

def tripleDesDecrypt(data,tripleDesKey = '11111111111111111111111111111111' ):
	try:
		t1 = triple_des(binascii.unhexlify(tripleDesKey),ECB,b'\0\0\0\0\0\0\0\0',pad=None ,padmode=PAD_NORMAL)
		result =  t1.decrypt(binascii.unhexlify(data))
	except ValueError as e:
		logging.error('in tripleDesDecrypt :%s' % e)
		return None
	return (binascii.hexlify(result).decode().upper())

def StrXor(str1 = '',str2 = ''):
	result = ''
	if (not isinstance(str1,str)) or (not isinstance(str2,str)) or (len(str1) != len(str2)):
		logging.error('error in StrXor ' )
		return None 
	lenStr = len(str1)
	if lenStr == 0:
		logging.error('input nonoe')
		return None
	#logging.info('str1:%s' % str1)
	#logging.info('str2:%s' % str2)

	try:
		str1Hex = binascii.unhexlify(str1)
		str2Hex = binascii.unhexlify(str2)
	except binascii.Error as e:
		logging.error('in StrXor:%s' % e)
		return None 
	except TypeError as e:
		logging.error('in StrXor:%s' % e)
		return None 

	if len(str1Hex) == len(str2Hex) :
		lenStr = len(str1Hex)
		for i in range(0,lenStr):
			result += '%02X'% ((int(str1Hex[i]) ^ int(str2Hex[i])))
		#logging.info('结果:%s\n' % result)
		return result
	


#计算pinblock
#
def GetPinblock(pin,cardno = '' ):
#	print(type(pin),type(cardno))
	result = ''
	if (not isinstance(pin,str)) or not isinstance(cardno,str):
		logging.error('error in GetPinblock ' )
		return None 
	lenOfPin = len(pin)
	lenOfCard = len(cardno)
	#密码长度为6~12位
	if lenOfPin < 6 or lenOfPin > 12:
		logging.error('error in GetPinblock : pin len error' )
		return None
	else:
		pinBlock = '%02d' % lenOfPin + pin + 'F' * (14 - lenOfPin) 
		pinHex = binascii.unhexlify(pinBlock)
		logging.info('密码:%s' % pinBlock)
	#卡号长度:15 ~ 19
	if not (lenOfCard ==0)and( lenOfCard < 15 or lenOfCard > 19):
		logging.error('error in GetPinblock : cardno len error' )
		return None
	elif lenOfCard == 0:
		logging.info('cardno is none ' )
		cardHex = ''
		return pinBlock
	else:
		#取卡号的最右１２位(不包括最右边的校验位)
		cardnoStr = '0' * 4 + cardno[-13:-1]
		cardHex = binascii.unhexlify(cardnoStr)
		logging.info('卡号:%s' % cardnoStr)

	if len(cardHex) == 8 and (len(cardHex) == len(pinHex) ):
		for i in range(0,8):
			result += '%02X'% ((int(cardHex[i]) ^ int(pinHex[i])))
		logging.info('结果:%s' % result)
		return result
	elif len(cardHex) == 0:
		logging.info('no cardno')
		return pinBlock
	else:
		logging.error('pinblock source data error')
		return None
	#logging.info(binascii.hexlify(result.encode()))
	#logging.info('结果:%s' % result)
	pass



#计算PIN加密块(最终的52域)
def GetPinblock3Des(pin,flag = 1 ,cardNo=''):
	if isinstance(flag,int) and flag ==1:
		pinblock = GetPinblock(pin,cardNo)	
	elif isinstance(flag,int) and flag == 6:
		pinblock = GetPinblock(pin)
	else:
		logging.error('can\'t support pinblock type: %d ' % flag)
		return 0
	if len(pinblock) == 0:
		return None
	if len(myConf.tmk) == 16:
		tpkDec = DesDecrypt(myConf.tpk,myConf.tmk )
	elif len(myConf.tmk) == 32:
		tpkDec =  tripleDesDecrypt(myConf.tpk,myConf.tmk )
	#logging.info('tmk = [%s],tmp = [%s]' % (myConf.tpk,myConf.tmk) )
	if tpkDec == None:
		return None
	#logging.info('tpkDec = [%s],pinblock = [%s]' % (tpkDec,pinblock) )
	if len(myConf.tmk) == 16:
		result =  DesEncrypt(pinblock,tpkDec)
	elif len(myConf.tmk) == 32:
		result =  tripleDesEncrypt( pinblock,tpkDec )
	if result == None:
		logging.error('get pinblock error')
	return result

#循环异或，传入list

def ListXor(MAB):
	result = ''
	logging.info(MAB)
	for index in range(len(MAB)):
		if index == 2: 
			result = StrXor(MAB[index],StrXor(MAB[index -2],MAB[index - 1])  )
		elif index > 2:
			result = StrXor(MAB[index],result )
		else:
			continue
	return result




def GenerateTermMac(macData):
	logging.info('macData = [%s]' % macData )
	if not (isinstance(macData,str) and len(macData) > 0):
		logging.error('the macData error')
		return None
	#补齐8的倍数字节
	macData += '0' * (16 - len(macData) % 16)
	if not (len(macData) % 16 == 0):
		logging.error('tail 0 add error' )
		return None
	logging.info(len(macData))
	#拆分
	MAB = re.findall(r'.{16}',macData)
	if len(MAB) == 0:
		logging.error('tail 0 add error' )
		return None
	#进行异或
	result = ListXor(MAB)
	if len(result) == 0:
		logging.error('XOR error' )
		return None
	#扩展一下
	resultHex = binascii.hexlify(result.encode()).decode()
	logging.info('resultHex = %s' % resultHex)
	#前半部分用MACKEY明文加密
	if len(myConf.tmk) == 32:
		TAK = tripleDesDecrypt(myConf.tak,myConf.tmk )
	elif len(myConf.tmk) == 16:
		TAK = DesDecrypt(myConf.tak,myConf.tmk )
	if TAK == None:
		return None
	logging.info('TAK = %s' % TAK)
	ENCBlock =  DesEncrypt(resultHex[0:16],TAK)
	if ENCBlock == None:
		return None
	logging.info('ENCBlock = %s' % ENCBlock)
	#后半部分与前半部分的密文进行异或
	ENCBlock2 = StrXor(ENCBlock,resultHex[16:])
	logging.info('ENCBlock2 = %s' % ENCBlock2)
	MAC =  DesEncrypt(ENCBlock2,TAK)
	logging.info('MAC = %s' % MAC)
	if not (isinstance(MAC,str) and len(MAC) == 16):
		return None
	else:
		return MAC[0:8]



'''
02 00 30 20 04 C0 20 C0 98 11 00 00 00 00 00 00
00 00 51 00 18 34 02 10 00 06 32 43 92 26 00 09
94 28 20 D1 31 21 01 12 00 84 39 31 36 36 38 38
30 31 34 30 30 30 30 30 30 30 30 30 30 30 30 30
30 31 31 35 36 AC F4 6E 73 83 8A 82 F9 26 00 00
00 00 00 00 00 00 08 22 00 00 01               
8A9E91B88D839AC5
8A9E91B8
'''
#logging.info(GetPinblock('123456'))
#logging.info(tripleDesEncrypt('06123456FFFFFFFF','0DFEDFE39E02F8A7BC46CB67790BDA5D' ))
#a = tripleDesEncrypt('06123456FFFFFFFF','0DFEDFE39E02F8A7BC46CB67790BDA5D' )
#logging.info(tripleDesDecrypt(a,'0DFEDFE39E02F8A7BC46CB67790BDA5D' ))
logging.info(GetPinblock3Des('000000',1,'4392260009942820'))
#logging.info(GenerateTermMac('0200302004C020C0981100000000000000005100183402100006324392260009942820D1312101120084393136363838303134303030303030303030303030303031313536ACF46E73838A82F92600000000000000000822000001' ))
#logging.info(StrXor('9A11302004C020C0','0000510018340210'))

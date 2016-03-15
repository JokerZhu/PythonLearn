#!/usr/bin/python3.4
#-*- coding:utf8 -*-
import binascii
import logging
import myConf
from pyDes import * 
import re
import hsmSvr
import mysqlInterFace


#循环异或，传入list
def ListXor(MAB):
	result = ''
	#logging.info(MAB)
	for index in range(len(MAB)):
		if index == 2: 
			result = StrXor(MAB[index],StrXor(MAB[index -2],\
			MAB[index - 1])  )
		elif index > 2:
			result = StrXor(MAB[index],result )
		else:
			continue
	return result

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


def StrXor(str1 = '',str2 = ''):
	result = ''
	if (not isinstance(str1,str)) or (not isinstance(str2,str)) \
		 or (len(str1) != len(str2)):
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

def GenTermMacPOSP(macData= '',packTmp = {}):

	#查找数据库，找出SEK和TAKBYSEK
	try:
		if not mysqlInterFace.ReadTermInfo(packTmp[42].decode(),\
			packTmp[41].decode() ):
			logging.error('can\'t found term info')
			packTmp[39] = '03'
			return -1 
	except KeyError as e:
		logging.error('error package')
		packTmp[39] = '30'
		return -1 
	SEK = mysqlInterFace.TermInfo['sekIndex']
	TAK = mysqlInterFace.TermInfo['takBySek']
	if SEK == None:
		SEK = myConf.SEK
	if TAK == None:
		packTmp[39] = '01'
		logging.error('Can\'t found tak' )
		return -1 


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
	#logging.info('result = %s' % result)
	resultHex = binascii.hexlify(result.encode()).decode()
	#logging.info('resultHex = %s' % resultHex)
	#前半部分用MACKEY明文加密
	result = hsmSvr.GenMACPOSP(result[0:8],SEK,TAK)
	#logging.info(result)
	if result[0] == '00':
		ENCBlock = StrXor(resultHex[16:],result[2] )
		logging.info('ENCBlock = %s ' % ENCBlock)
	else:
		return None

	result = hsmSvr.GenMACPOSP(binascii.a2b_hex(ENCBlock),SEK,TAK)

	return result
	pass
def CheckTermMacPOSP(macData = '',packTmp = {} ):
	try:
		if isinstance(packTmp[64],bytes):
			MacRecv = packTmp[64].decode()
	except KeyError: 
		logging.info('can\'t found mac in package')
		packTmp[39] = '30'
		return -1
		
	
	result = GenTermMacPOSP(macData[0:len(macData) -16],packTmp)
	if isinstance(result,int) and result <= 0:
		logging.info('Mac check error')
		return -1
	
	logging.info('localMac = [%s]' % result[2])
	logging.info('MacRecv  = [%s]' % MacRecv)
	if result[2][0:8] == MacRecv:
		logging.info('Mac check OK')
		packTmp[39] = '00'
	else:
		logging.info('Mac check error')
		packTmp[39] = 'A0'
		return -1
	pass
#判断上送的pinblock是否正确
def CheckTermPinPosp():
	pass

# 01D122D10BC7EB28 0DFEDFE39E02F8A7BC46CB67790BDA5D   4392260009942820 000000
GetPinblock('123456')

#GenTermMacPOSP('0200602006C020C08A11166214441000010053310000827656052000050006296214441000010053D30102200000003333313530303232383132333331353435313130303134313536260000000000000001129F2608BBFD1BFACF6A64299F2701809F100807000103A0A002019F3704989636539F36020334950500800088009A031603099C01319F02060000000000005F2A02015682023C009F1A0201569F03060000000000009F3303E0E1C89F34031E03009F3501229F1E083132333435363738001301000001000500','8A9E91B8')

#!/usr/bin/python3.4
#-*- coding:utf8 -*-
import binascii
import logging
import myConf
from pyDes import * 



#计算pinblock
#
def GetPinblock(pin,cardno = '1234567890123456' ):
#	print(type(pin),type(cardno))
	if (not isinstance(pin,str)) or not isinstance(cardno,str):
		logging.error('error in GetPinblock ' )
		return None 
	lenOfPin = len(pin)
	lenOfCard = len(cardno)
	#密码长度为6~12位
	if lenOfPin < 6 or lenOfPin > 12:
		logging.error('error in GetPinblock : pin len error' )
		return None
	pinBlock = '%02d' % lenOfPin + pin + 'F' * (14 - lenOfPin) 
	#卡号长度:15 ~ 19
	if not (lenOfCard ==0)and( lenOfCard < 15 or lenOfCard > 19):
		logging.error('error in GetPinblock : cardno len error' )
		return None
	else:
		#取卡号的最右１２位(不包括最右边的校验位)
		cardnoStr = '0' * 4 + cardno[-13:-1]
	logging.info('密码:%s' % pinBlock)
	logging.info('卡号:%s' % cardnoStr)
	pinHex = binascii.hexlify(pinBlock.encode('ascii'))
	cardHex = binascii.hexlify(cardnoStr.encode('ascii'))
	logging.info(pinHex)	
	logging.info('结果:%s' % cardnoStr)
	pass



#计算PIN加密块(最终的52域)
def GetPinblock3Des():
	pass




GetPinblock('1234563433')

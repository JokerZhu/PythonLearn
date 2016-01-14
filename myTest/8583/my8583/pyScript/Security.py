#!/usr/bin/python3.4
#-*- coding:utf8 -*-
import binascii
import logging
import myConf
from pyDes import * 



#计算pinblock
#
def GetPinblock(pin,cardno = '' ):
	print(type(pin),type(cardno))
	if (not isinstance(pin,str)) or not isinstance(cardno,str):
		logging.error('error in GetPinblock ' )
		return 0
	lenOfPin = len(pin)
	lenOfCard = len(cardno)
	#密码长度为6~12位
	if lenOfPin < 6 or lenOfPin > 12:
		logging.error('error in GetPinblock : pin len error' )
		return 0
	#卡号长度:15 ~ 19
	#if lenOfCard 


	pinBlock = '%02d' % lenOfPin + pin + 'F' * (14 - lenOfPin) 
	logging.info(pinBlock)
	
	
	pass



#计算PIN加密块(最终的52域)
def GetPinblock3Des():
	pass




#GetPinblock('1234563433')

#!/usr/bin/python3.4
#-*- coding:utf8 -*-
import myConf
import logging
import os
import time
import qrcode
import qrcode.image.svg
from PIL import Image
import re
import readline




def GetSerial():
	initSeq = '1'
	#从配置文件取出流水号
	cfgFile = myConf.GetCombination('app_env','CfgDir','usr_var','seqFile')
	logging.info('seq file = [%s]' % cfgFile)
	try:
		with open(cfgFile,'a+') as seqFile:
			seqFile.seek(0)
			allLines = seqFile.readlines()
			if len(allLines) != 1 :
				logging.info('seq file format error')
				seqFile.seek(0)
				seqFile.truncate()  
				seqFile.writelines(initSeq )
			else:
				try:
					if int(allLines[0]) + 1 > 999999: 
						seq = '%06d' % int(initSeq)   
					else:
						seq = '%06d' % int(allLines[0])   
					seqFile.seek(0)
					seqFile.truncate()  
					seqFile.writelines(str(int(seq ) + 1) )
				except ValueError as e:
					seq = '%06d' % int(initSeq)   
					seqFile.seek(0)
					seqFile.truncate()  
					seqFile.writelines(str(int(seq ) + 1) )
				
	except FileNotFoundError as e:
		logging.info("can\'t open file [%s] " % cfgFile )
	#logging.info("seq = [%s]" % seq )
	
	return seq 
def GetLocalDate():
	T = time.localtime()
	localDate = '%02d' % T[1] + '%02d' % T[2]
	logging.info('localDate = [%s] ' % localDate)
	return localDate 

def GetLocalTime():
	T = time.localtime()
	localTime = '%02d' % T[3] + '%02d' % T[4] + '%02d' % T[5]
	logging.info('localTime = [%s] ' % localTime)
	return localTime 

def CallCustomFun(functionName ):
	logging.info('in CallCustomFun index = [%s]' % functionName)
	result =  OperatorOfFun.get(functionName)()
	return result

def SetDefultValue(value ):
	logging.info('in SetDefultValue  = [%s]' % value)
	return value
	pass
def CallInputFun(txt = '' ):
	logging.info('in InPutFun!!!')
	pressData = input('请输入' + txt + ':\n')
	return pressData
	pass

def CallInputQRFun(txt = '' ):
	logging.info('in InPutFun!!!')
	pressData = input('请输入' + txt + ':\n')
	return 'QRnumber=' + pressData 
	pass


def AutoSetFld(packSource = []):
	if not isinstance(packSource[1],str) :
		logging.error('this cfg %s is error' % packSource)
		return None
	try:
		value = myOperator.get(packSource[0])(packSource[1])
	except TypeError as e:
		logging.error('not support this cfg %s' % packSource)
		return None
	return value
def CreateQrcode(sourceMsg ='alipaySeq=&QRlink='):
	sorceData = []
	if (not isinstance(sourceMsg,str)) and len(sourceMsg) <= 0:
		logging.error('can\'t create qrcode!')
		return 0
	sorceData =  re.findall(r'alipaySeq=(\d{0,20})&', sourceMsg)
	sorceData += re.findall(r'QRlink=(.{0,128})$' ,sourceMsg)
	
	if len(sorceData) != 2:
		logging.error('can\'t create qrcode!')
		return 0
	cmd = 'qr %s' % (sorceData[1])
	os.system(cmd) 	
	input("press <enter> to continue")

myOperator = {
				'Def':SetDefultValue,
				'Fun':CallCustomFun,
				'InPut':CallInputFun,
				'InPutqr':CallInputQRFun 
			}

OperatorOfFun = {
					'GetSerial':GetSerial,
					'GetDate':GetLocalDate,
					'GetTime':GetLocalTime,
	 			}

#logging.info(AutoSetFld(['Def','234']))
#logging.info(AutoSetFld([1,'Fun','GetSerial']))
#logging.info(AutoSetFld(['Fun','GetDate']))
#GetLocalDate()
#GetLocalTime()






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
import pack8583
from ctypes import *
import Security




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
def GenTermMac():
	logging.info('in GenTermMac')	
	pack8583.setPackageFlf(64,'00000000')

	tmpStr = create_string_buffer(1024)
	Len = pack8583.libtest.packageFinal(tmpStr)
	#logging.info('len = [%d] after pack = [%s]' %(Len ,tmpStr.value))
	MAC = Security.GenerateTermMac(tmpStr.value.decode()[:-16]) 
	logging.info(MAC)
	return MAC 
def GetCardNoFromPackage():
	tmpStr = create_string_buffer(128)
	length = pack8583.libtest.getFldValue(2,tmpStr,sizeof(tmpStr))
	if length == 0:
		length = pack8583.libtest.getFldValue(35,tmpStr,sizeof(tmpStr))
		if length > 0:
			cardno = re.findall(r'(\d{15,21})[D,=]',tmpStr.value.decode())[0]
		else:
			return None
	else:
		return None
	logging.info('cardno = %s ' % cardno)
	
	return cardno 

def InPutPW(flag='' ):
	logging.info('in input passwd fun!!')
	if not isinstance(flag,str):
		logging.error('input passwd error!')
		return None
	flagLen = len(flag)
	#配置文件直接赋值
	if flagLen >= 6 and flagLen <= 12 and flag.isdigit():
		passwd = flag 
		withcardno = False
		pinblock = Security.GetPinblock3Des(passwd,)
	elif flag == 'withcardno':
		withcardno = True 
		inputPasswd = input('请输入您的密码:\n')
		if len(inputPasswd) >= 6 and len(inputPasswd) <= 12 and inputPasswd.isdigit():
			passwd = inputPasswd 
			cardNo = GetCardNoFromPackage()
		else:
			logging.error('you input passwd error')
			return None
		pinblock = Security.GetPinblock3Des(passwd,1,cardNo)
	else:
		return None
	logging.info('pinblock = [%s]' % pinblock)
	return pinblock
def InPutPWWithCard(flag='' ):
	logging.info('in input passwd fun!!')
	if not isinstance(flag,str):
		logging.error('input passwd error!')
		return None
	flagLen = len(flag)
	cardNo = GetCardNoFromPackage()
	#配置文件直接赋值
	if flagLen >= 6 and flagLen <= 12 and flag.isdigit():
		passwd = flag 
		withcardno = False
		pinblock = Security.GetPinblock3Des(passwd,1,cardNo)
	elif flag == '':
		withcardno = True 
		inputPasswd = input('请输入您的密码:\n')
		if len(inputPasswd) >= 6 and len(inputPasswd) <= 12 and inputPasswd.isdigit():
			passwd = inputPasswd 
		else:
			logging.error('you input passwd error')
			return None
		pinblock = Security.GetPinblock3Des(passwd,1,cardNo)
	else:
		return None
	logging.info('pinblock = [%s]' % pinblock)
	return pinblock

def InPutPWNoCard(flag='' ):
	logging.info('in input passwd fun!!')
	if not isinstance(flag,str):
		logging.error('input passwd error!')
		return None
	flagLen = len(flag)
	#配置文件直接赋值
	if flagLen >= 6 and flagLen <= 12 and flag.isdigit():
		passwd = flag 
		withcardno = False
		pinblock = Security.GetPinblock3Des(passwd)
	elif flag == '':
		withcardno = True 
		inputPasswd = input('请输入您的密码:\n')
		if len(inputPasswd) >= 6 and len(inputPasswd) <= 12 and inputPasswd.isdigit():
			passwd = inputPasswd 
		else:
			logging.error('you input passwd error')
			return None
		pinblock = Security.GetPinblock3Des(passwd)
	else:
		return None
	logging.info('pinblock = [%s]' % pinblock)
	return pinblock
	
myOperator = {
				'Def':SetDefultValue,
				'Fun':CallCustomFun,
				'InPut':CallInputFun,
				'InPutqr':CallInputQRFun,
				'InPutPWWithCard':InPutPWWithCard,
				'InPutPWNoCard':InPutPWNoCard,
			}

OperatorOfFun = {
					'GetSerial':GetSerial,
					'GetDate':GetLocalDate,
					'GetTime':GetLocalTime,
					'GenMac':GenTermMac,
	 			}

#logging.info(AutoSetFld(['Def','234']))
#logging.info(AutoSetFld([1,'Fun','GetSerial']))
#logging.info(AutoSetFld(['Fun','GetDate']))
#GetLocalDate()
#GetLocalTime()






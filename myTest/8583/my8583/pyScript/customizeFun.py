#!/usr/bin/python3.4
#-*- coding:utf8 -*-
import myConf
import logging
import os
import time




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


myOperator = {'Def':SetDefultValue,'Fun':CallCustomFun }
OperatorOfFun = {'GetSerial':GetSerial,
				'GetDate':GetLocalDate,
				'GetTime':GetLocalTime,
	 			}

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


#logging.info(AutoSetFld(['Def','234']))
#logging.info(AutoSetFld([1,'Fun','GetSerial']))
#logging.info(AutoSetFld(['Fun','GetDate']))
#GetLocalDate()

#GetLocalTime()

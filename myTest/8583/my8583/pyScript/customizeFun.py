#!/usr/bin/python3.4
#-*- coding:utf8 -*-
import myConf
import logging



def GetSerial():
	return '123456'
def GetDate():
	return 'date'

def CallCustomFun(index,functionName ):
	logging.info('in CallCustomFun index = [%d]' % index)
	result =  OperatorOfFun.get(functionName)()
	return result

def SetDefultValue(index,value ):
	logging.info('in SetDefultValue index = [%d]' % index)
	return value
	pass


myOperator = {'Def':SetDefultValue,'Fun':CallCustomFun }
OperatorOfFun = {'GetSerial':GetSerial,
				'GetDate':GetDate
	 			}

def AutoSetFld(packSource = []):
	if (not isinstance(packSource[0] ,int)) or ( not isinstance(packSource[2],str) ):
		logging.error('this cfg %s is error' % packSource)
		return None
	try:
		value = myOperator.get(packSource[1])(packSource[0],packSource[2])
	except TypeError as e:
		logging.error('not support this cfg %s' % packSource)
		return None
	return value


#logging.info(AutoSetFld([1,'Def','234']))
#logging.info(AutoSetFld([1,'Fun','GetSerial']))
#logging.info(AutoSetFld([1,'Fun','GetDate']))

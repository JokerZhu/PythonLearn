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
import mysqlInterFace
import hsmSvr




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

def GetSysNo():
	T = time.localtime()
	localTime = '%02d' % T[3] + '%02d' % T[4] + '%02d' % T[5]
	return localTime + pack8583.package[11].decode()

def InqHandle():
	return '00' 

def CheckInq60():
	return '' 
def CheckMac():

	return '' 

def CallCustomFun(functionName):
	logging.info('in CallCustomFun index = [%s]' % functionName)
	result =  OperatorOfFun.get(functionName)()
	return result

def SetDefultValue(value):
	logging.info('in SetDefultValue  = [%s]' % value)
	return value
	pass


def AutoSetFld(packSource = []):
	if not isinstance(packSource[1],str) :
		logging.error('this cfg %s is error' % packSource)
		return '' 
	#logging.info('packSource = ',packSource)
	if packSource[0] == 'R':
		return ReturnASRev(packSource[1])
	try:
		value = myOperator.get(packSource[0])(packSource[1])
	except TypeError as e:
		logging.error('not support this cfg %s' % packSource)
		return '' 
	return value
	pass

	
def GenTermWorkLey():
	logging.info('GenTermWorkLey')
	logging.info(pack8583.package)
	tmkLen = [16,32,48]
	#read term table
	try:
		where = 'tid = \'%s\' and mid = \'%s\' limit 1' % \
		(pack8583.package[41].decode(),pack8583.package[42].decode() )
	except KeyError as e:
		logging.error(e)
		pack8583.setPackageFlf(39,'01')
		return None
	result = mysqlInterFace.SelFormDB('*','term',where )
	logging.info('result = ', result[0])
	if isinstance(result,int) and result == 0:
		pack8583.setPackageFlf(39,'03')
		return None
	tmk = result[0]['tmk']
	logging.info(result)
	if result[0]['status'] == '0' or not isinstance(tmk,str):
		pack8583.setPackageFlf(39,'03')
		return None
	#print(result[0].get('tmk'))
	
	if len(result[0]['tmk']) not in tmkLen:
		pack8583.setPackageFlf(39,'03')
		return None

	#genPink
	if isinstance(result[0]['sekIndex'],str) and len(result[0]['sekIndex']) > 0:
		tpkRsult =  hsmSvr.GenTermPinKey(tmk,result[0]['sekIndex'])
	else:
		tpkRsult =  hsmSvr.GenTermPinKey(tmk)
	logging.info(tpkRsult)
	if tpkRsult[0] != '00':
		pack8583.setPackageFlf(39,'03')
		return None
	#genMackey
	if isinstance(result[0]['sekIndex'],str) and len(result[0]['sekIndex']) > 0:
		takRsult =  hsmSvr.GenTermMacKey(tmk,result[0]['sekIndex'])
	else:
		takRsult =  hsmSvr.GenTermMacKey(tmk)
	if tpkRsult[0] != '00':
		pack8583.setPackageFlf(39,'03')
		return None
	logging.info(takRsult)
	#update to mysql
	mysqlInterFace.UpdateToDB('tpk = \'%s\',tpkBySek = \'%s\',tak = \'%s\',takBySek = \'%s\',loginTime = NOW() '
		 %(tpkRsult[2],tpkRsult[1],takRsult[2],takRsult[1]) ,'term',where  )

	return tpkRsult[2] + tpkRsult[3][0:8] + takRsult[2] +'0' * 16 + takRsult[3][0:8]
	pass


def ReturnASRev(fld = ''):
	logging.info("ReturnASRev")
	try:
		if isinstance(pack8583.package[int(fld)],bytes ):
			valueSet = str(pack8583.package[int(fld)].decode('iso-8859-15'))
		elif isinstance(pack8583.package[int(fld)],str):
			valueSet = pack8583.package[int(fld)]
		return valueSet
	except TypeError as e:
		logging.error('ReturnASRev : ', e)
		return ''
	except KeyError as e:
		logging.error('ReturnASRev %s : ' % e)
		return ''

def CheckSeq():
	logging.info('in CheckSeq')
	try:
		where = 'b41 = \'%s\' and b42 = \'%s\' and b11 = \'%s\' limit 1' % \
		(pack8583.package[41].decode(),pack8583.package[42].decode(),pack8583.package[11].decode() )
	except KeyError as e:
		logging.error(e)
		#pack8583.setPackageFlf(39,'30')
		return None
	result = mysqlInterFace.SelFormDB('*','trans',where )

	if isinstance(result,int) and result == 0:
		logging.info('seq check ok')
	elif isinstance(result,list) and len(result) > 0:
		logging.error('seq repeat')
		pack8583.package[39] = '94'
	else:
		logging.error('seq repeat')
		pack8583.package[39] = '94'
	pass

def NominalReq():
	defaultResp = '00'
	try:
		respCode = pack8583.package[39] 
	except KeyError :
		logging.info('set default respcode')
		return defaultResp
	if (not isinstance(respCode,str)) or len(respCode) != 2 :
		logging.info('set default respcode')
		return defaultResp
	else:
		return respCode

myOperator = {
				'Def':SetDefultValue,
				'Fun':CallCustomFun,
			}

OperatorOfFun = {
					'GetDate':GetLocalDate,
					'GetTime':GetLocalTime,
					'GenTermWorkLey':GenTermWorkLey,
					'GetSysNo':GetSysNo,
					'InqHandle':InqHandle,
					'CheckInq60':CheckInq60,
					'CheckMac':CheckMac,
					'CheckSeq':CheckSeq,
					'NominalReq':NominalReq,
	 			}

#logging.info(AutoSetFld(['Def','234']))
#logging.info(AutoSetFld([1,'Fun','GetSerial']))
#logging.info(AutoSetFld(['Fun','GetDate']))
#GetLocalDate()
#GetLocalTime()






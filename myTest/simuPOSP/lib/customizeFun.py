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




def GetLocalDate(revDir = {}):
	T = time.localtime()
	localDate = '%02d' % T[1] + '%02d' % T[2]
	logging.info('localDate = [%s] ' % localDate)
	return localDate 

def GetLocalTime(revDir = {}):
	T = time.localtime()
	localTime = '%02d' % T[3] + '%02d' % T[4] + '%02d' % T[5]
	logging.info('localTime = [%s] ' % localTime)
	return localTime 

def GetSysNo(revDir = {}):
	T = time.localtime()
	localTime = '%02d' % T[3] + '%02d' % T[4] + '%02d' % T[5]
	return localTime + revDir[11].decode()

def InqHandle(revDir = {}):
	return '00' 

def CheckInq60(revDir = {}):
	return '' 
def CheckMac(revDir = {}):

	return '' 

def CallCustomFun(functionName,revDir={} ):
	logging.info('in CallCustomFun index = [%s]' % functionName)
	result =  OperatorOfFun.get(functionName)(revDir)
	return result

def SetDefultValue(value,revDir = {} ):
	logging.info('in SetDefultValue  = [%s]' % value)
	return value
	pass


def AutoSetFld(packSource = [],revDir = {}):
	if not isinstance(packSource[1],str) :
		logging.error('this cfg %s is error' % packSource)
		return None
	#logging.info('packSource = ',packSource)
	if packSource[0] == 'R':
		return ReturnASRev(revDir,packSource[1])
	try:
		value = myOperator.get(packSource[0])(packSource[1],revDir)
	except TypeError as e:
		logging.error('not support this cfg %s' % packSource)
		return None
	return value
	pass

	
def GenTermWorkLey(revDir = {}):
	logging.info('GenTermWorkLey')
	logging.info(revDir)
	tmkLen = [16,32,48]
	#read term table
	try:
		where = 'tid = \'%s\' and mid = \'%s\' limit 1' % \
		(revDir[41].decode(),revDir[42].decode() )
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


def ReturnASRev(revDir = {},fld = ''):
	logging.info("ReturnASRev")
	try:
		if isinstance(revDir[int(fld)],bytes ):
			valueSet = str(revDir[int(fld)].decode('iso-8859-15'))
		elif isinstance(revDir[int(fld)],str):
			valueSet = revDir[int(fld)]
		return valueSet
	except TypeError as e:
		logging.error('ReturnASRev : ', e)
		return ''
	except KeyError as e:
		logging.error('ReturnASRev %s : ' % e)
		return ''


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
	 			}

#logging.info(AutoSetFld(['Def','234']))
#logging.info(AutoSetFld([1,'Fun','GetSerial']))
#logging.info(AutoSetFld(['Fun','GetDate']))
#GetLocalDate()
#GetLocalTime()






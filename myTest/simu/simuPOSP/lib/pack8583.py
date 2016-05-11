#!/usr/bin/python3.4
#-*- coding:utf8 -*-

from ctypes import *
import logging
import os
import re
import socket
import binascii
import configparser
import customizeFun
import myConf
import sys
import time
import Security

libtest = cdll.LoadLibrary(myConf.GetLibName())

def setPackageFlf(n = 0,data='' ):
	hexliset = [62,55]
	if (not isinstance(n,int)) and (not isinstance(data,str)):
		logging.error('error parameter')
		return 0 
	tmpStr = create_string_buffer(1024)
	memset(tmpStr,0,sizeof(tmpStr))
	tmpStr.value = bytes(data.encode()) 
	if n in hexliset:
		libtest.packageSetHex(n,tmpStr)
	else:
		libtest.packageSet(n,tmpStr)
	#libtest.packageSet(62,bytes(bytes('08F859DB0F61C4FCBC15AF3642172861'.encode())))
	pass
def packageINIT():
	packageDef = create_string_buffer(bytes(myConf.term8583.encode()),128)
	packageDir = create_string_buffer(bytes(myConf.packageDir.encode()),128)
	ret = libtest.packageInit(packageDir,packageDef )
	if ret < 0:
		logging.error('8583 package init error')
		return Flase 
	else:
		return True


def packPackage8583(transName):
	Len = 0
	global packageRes
	valueSet = ''
	packageRes = {}
	packageDef = create_string_buffer(bytes(myConf.term8583.encode()),128)
	packageDir = create_string_buffer(bytes(myConf.packageDir.encode()),128)
	ret = libtest.packageInit(packageDir,packageDef )
	if ret < 0:
		logging.error('8583 package init error')
		return None 
	tmpStr = create_string_buffer(1024*2)
	tmp = create_string_buffer(1024)
	memset(tmpStr,0,sizeof(tmpStr))


	try:
		for each in myConf.allTransCfg[transName]:
			logging.info(each)
			valueSet = customizeFun.AutoSetFld([each[1],each[2]])
			if valueSet != None and len(valueSet) > 0:
				setPackageFlf(int(each[0]),valueSet)
	except KeyError as e:
		logging.error("pack error ,transName error")
	logging.info('pack finished')

	for i in range(0,128):
	#	logging.info('i = [%d] typeof(i) = [%s]' % (i,type(i)))
		Len = libtest.getFldValue(i,tmp,sizeof(tmp))
		if Len <= 0:
			continue
		logging.info('[%04d][%04d][%s]' % (i, len(tmp.value),tmp.value))
	Len = libtest.packageFinal(tmpStr);	
	libtest.packFree();	
	logging.info('len = [%d] after pack = [%s]' %(Len ,tmpStr.value))
	#打报文头
	#bcd= binascii.a2b_hex(bytes(myConf.packageHeader.encode())) +  binascii.a2b_hex(tmpStr.value)
	#不打报文头
	bcd= (tmpStr.value)
	#bcd= binascii.a2b_hex(tmpStr.value)

	return bcd
	pass

def unpack8583(backData):
	global package 
	fld0 = create_string_buffer(5)
	fld3 = create_string_buffer(7)
	fld59 = create_string_buffer(1024)
	fld62 = create_string_buffer(1024)
	Len = 0
	package = {} 
	backPackage = create_string_buffer(1024*2)
	backPackage.value = backData
	logging.info('befor unpack = [%s],len = [%d]' %  (backPackage.value,len(backPackage.value ) ) )
	logging.info('len = [%d]' %  (len(backPackage.value ) ) )
	#print('backpackage len = %d' % len(backPackage.value))
	length = c_int(len(backPackage.value))
	#logging.info('backPackage.value = [%s]' % backPackage.value)
	packageDef = create_string_buffer(bytes(myConf.term8583.encode()),128)
	packageDir = create_string_buffer(bytes(myConf.packageDir.encode()),128)
	ret = libtest.packageInit(packageDir,packageDef )
	if ret < 0:
		logging.error('8583 package init error')
	#libtest.unpack8583(packageDir,backPackage,len(backPackage.value))
	ret = libtest.unpack8583(packageDir,backPackage,length.value)
	if ret < 0:
		logging.info('unpack error ret = %d ' % ret)
		return None

	
	logging.info('unpack ok ')
	tmp = create_string_buffer(1024)
	for i in range(0,128):
		Len = libtest.getFldValue(i,tmp,sizeof(tmp))
		if Len <= 0:
			continue
		logging.info('[%04d][%04d][%s]' % (i, len(tmp.value),tmp.value))
		#package.append(i,tmp.value)
		package[i] = tmp.value
	logging.info('unpack end')
	libtest.unpackFinal()
	return True 
	#return package

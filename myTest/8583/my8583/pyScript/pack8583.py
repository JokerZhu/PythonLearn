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
	if (not isinstance(n,int)) and (not isinstance(data,str)):
		logging.error('error parameter')
		return 0 
	tmpStr = create_string_buffer(1024)
	memset(tmpStr,0,sizeof(tmpStr))
	tmpStr.value = bytes(data.encode()) 
	libtest.packageSet(n,tmpStr)
	pass


def packPackage8583(transName):
	Len = 0
	valueSet = ''
	packageDef = create_string_buffer(bytes(myConf.term8583.encode()),128)
	packageDir = create_string_buffer(bytes(myConf.packageDir.encode()),128)
	ret = libtest.packageInit(packageDir,packageDef )
	if ret < 0:
		logging.error('8583 package init error')
		return None 
	tmpStr = create_string_buffer(1024*2)
	tmp = create_string_buffer(1024)
	memset(tmpStr,0,sizeof(tmpStr))
	cfgFile = myConf.GetCombination('app_env','CfgTransDef','trans_type',transName)
	logging.info('cfgFile = %s' % cfgFile )

	with open(cfgFile,'r') as fileCfg:
		AllLines = fileCfg.readlines()
		#logging.info(AllLines) 
		for line in AllLines:
			if line[0] == '#' or line[0] == '\n':
				continue
			else:
			#l = re.findall(r'^\[(\d{4})\]',line)
				valueWay = []
				line = line.strip('\n')
				line = line.replace('][',',')
				line = line.replace(']','')
				line = line.replace('[','')
				l = line.split(',')
				logging.info(l)
				valueWay.append(l[1])
				valueWay.append(l[2])

				valueSet = customizeFun.AutoSetFld(valueWay)
				if valueSet != None and len(valueSet) > 0:
					setPackageFlf(int(l[0]),valueSet)
				#setPackageFlf(int(l[0]),customizeFun.AutoSetFld(valueWay))

	'''
	setPackageFlf(0,'0800')
	setPackageFlf(11,'000001')
	setPackageFlf(41,'00000001')
	setPackageFlf(42,'000000000000001')
	setPackageFlf(60,'00000025031')
	setPackageFlf(63,'80')
	'''
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
	bcd= binascii.a2b_hex(bytes(myConf.packageHeader.encode())) +  binascii.a2b_hex(tmpStr.value)

	return bcd
	pass

def unpack8583(backData):
	fld0 = create_string_buffer(5)
	fld3 = create_string_buffer(7)
	fld59 = create_string_buffer(1024)
	Len = 0
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
	libtest.unpack8583(packageDir,backPackage,length.value)
	logging.info('unpack ok')
	tmp = create_string_buffer(1024)
	for i in range(0,128):
		Len = libtest.getFldValue(i,tmp,sizeof(tmp))
		if Len <= 0:
			continue
		logging.info('[%04d][%04d][%s]' % (i, len(tmp.value),tmp.value))
	logging.info('unpack end')
	##判断是否支付宝生成二维码交易，如果是则显示二维码
	libtest.getFldValue(0,fld0,sizeof(fld0))
	libtest.getFldValue(3,fld3,sizeof(fld3))
	libtest.getFldValue(59,fld59,sizeof(fld59))
	logging.info('%s %s %s' % (fld0.value,fld3.value,fld59.value));
	if fld0.value == b'0710' and fld3.value == b'170000':
		logging.info('create qrcode of alipay!')
		customizeFun.CreateQrcode(fld59.value.decode())
	return libtest.unpackFinal()
	pass

#!/usr/local/bin/python3.4
#-*- coding:utf8 -*-

from ctypes import *
import logging
import os
import re
import socket
import binascii
import configparser
import myConf
import sys
import time
import customizeFun
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
	'''
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
	for each in myConf.allTransCfg[transName]:
		logging.info(each)
		valueSet = customizeFun.AutoSetFld([each[1],each[2]])
		if valueSet != None and len(valueSet) > 0:
			setPackageFlf(int(each[0]),valueSet)
		 

	logging.info('pack finished')
	'''
	for i in range(0,128):
	#	logging.info('i = [%d] typeof(i) = [%s]' % (i,type(i)))
		Len = libtest.getFldValue(i,tmp,sizeof(tmp))
		if Len <= 0:
			continue
		logging.info('[%04d][%04d][%s]' % (i, len(tmp.value),tmp.value))
	'''
	Len = libtest.packageFinal(tmpStr);	
	libtest.packFree();	
	logging.info('len = [%d] after pack = [%s]' %(Len ,tmpStr.value))
	bcd= binascii.a2b_hex(bytes(myConf.packageHeader.encode("'iso-8859-15'" ))) +  binascii.a2b_hex(tmpStr.value)

	return bcd
	pass

def unpack8583(backData):
	fld0 = create_string_buffer(5)
	fld3 = create_string_buffer(7)
	fld59 = create_string_buffer(1024)
	fld39 = create_string_buffer(3)
	fld62 = create_string_buffer(1024)
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
	libtest.getFldValue(62,fld62,sizeof(fld62))
	libtest.getFldValue(39,fld39,sizeof(fld39))
	'''
	logging.info('%s %s %s' % (fld0.value,fld3.value,fld59.value));
	if fld0.value == b'0710' and fld3.value == b'170000':
		logging.info('create qrcode of alipay!')
		customizeFun.CreateQrcode(fld59.value.decode())
	'''
	if fld0.value == b'0810' or fld0.value == b'0910':
		customizeFun.SaveWorkKey(fld62.value.decode())
	return libtest.unpackFinal(),fld39.value.decode()
	pass

#压力测试专用打包函数
def packPackage8583ForStress(transName):
	packDef = {
		"消费":'0200303804C120C098110000000000000002006826950211520407002100120812999000356227001823260036733D000000000000000030303030303039393030303030303030303030303030313135368D75286DA9147AA026000000000000000014220000010005013030303030303030',
	}
	if transName == '消费':
		packageBase = packDef[transName] 
		logging.info("customizeFun.currentSeq  = [%d]" % customizeFun.currentSeq )
		#流水号
		package = packageBase[:38] + '%06d' %(customizeFun.currentSeq % 999999 )  + packageBase[44:]
		#时间
		package = package[:44] + customizeFun.GetLocalTime() + package[50:]
		#日期
		package = package[:50] + customizeFun.GetLocalDate() + package[54:]
		#终端号
		package = package[:110] + customizeFun.bcdhexToaschex(myConf.termid.encode()) + package[126:]
		#商户号
		package = package[:126] + customizeFun.bcdhexToaschex(myConf.mid.encode()) + package[156:]
		MAC = Security.GenerateTermMac(package[:-16])
		package = package[:-16] + customizeFun.bcdhexToaschex(MAC.encode())
		#print(MAC)



	return (package)
	

	pass



#unpack8583(b"0200302004C120C09811000000000000000100000501021000120800000001356227001823260036733D00000000000000003737373738383838313030303030303030303030303031313536533A4D1CA0F27BDC26000000000000000014220000010005013933313036384132")
#unpack8583( packPackage8583ForStress('消费'))

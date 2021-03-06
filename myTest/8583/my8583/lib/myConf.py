#!/usr/bin/python3.4
#-*- coding:utf8 -*-
import os
import sys
import configparser
import logging
import time

configName = sys.path[0]+'/../conf/appcfg.ini'


#从配置文件里读取一个配置
def ReadConf(sections,option,type = 'string'):
	conf = configparser.ConfigParser()
	result = None 
	if  (not isinstance(sections,str)) or (not isinstance(option,str)):
		logging.error('parameter error!!')
		return None
	try:
		conf.read(configName)
	#logging.info(conf.sections() )
	except configparser.Error as e:
		logging.error(e)
	if type == 'string':
		try:
			result = conf.get(sections,option )
		except configparser.NoSectionError as e:
			logging.error('can\'t find config sections:%s  option: %s,%s' %(sections,option,e)) 
		except configparser.NoOptionError  as e:
			logging.error('can\'t find config sections:%s  option: %s' %(sections,option)) 
		return result
	elif type == 'int':
		try:
			result = conf.getint(sections,option )
		except configparser.NoSectionError as e:
			logging.error('can\'t find config sections:%s  option: %s,%s' %(sections,option,e)) 
		except configparser.NoOptionError  as e:
			logging.error('can\'t find config sections:%s  option: %s' %(sections,option)) 
		return result
	else:
		logging.error('error config type : %s' %(type)) 
		return result

def SetConf(sections,option,value ):
	
	conf = configparser.ConfigParser()
	try:
		conf.read(configName)
	except configparser.Error as e:
		logging.error(e)
	try:
		conf.set(sections,option,value)
	except configparser.Error as e:
		logging.error(e)
	return conf.write(open(configName,'w'))
	 
	



#功能 : 读取配置文件 Section = trans_type下的配置
#返回 : 没有返回None，否则返回一个字典
def ReadAllTransType():
	result = []
	conf = configparser.ConfigParser()
	result = None 
	try:
		conf.read(configName)
	except configparser.Error as e:
		logging.error(e)
	allType = conf.options('trans_type')
	if len(allType ) == 0:
		logging.info('their is no transType ')
		return None
#	for each in allType:
#		logging.info('each = [%s] '% each )
#		result.append(each)
		#resultDict[each] = ReadConf('trans_type',each)
		#logging.info('resultDict = [%s] '% resultDict )
	return allType 
		
	
#根据规则获取日志名:LogTransName.年月日.log
def GetLogFileName():
	logName = ReadConf('app_env','LogTransName')
	logDir = ReadConf('app_env','LogDir')
	if logName == None or logDir == None:
		return None
	T = time.localtime()
	localDate = '%04d' % T[0] + '-'+'%02d' % T[1] +'-' + '%02d' % T[2]
	return sys.path[0] + '/'+ logDir +  logName + '.'+ localDate + '.log'

logging.basicConfig(level=logging.DEBUG,                                                                                                                                
    format='%(asctime)s [%(filename)s][line:%(lineno)d] [%(process)d] [%(levelname)s] %(message)s',
       datefmt='[%Y%m%d%H%M%S]',
                filename= GetLogFileName() ,
                #filename= './test.log' ,
                filemode='a+')
#两个配置文件的组合，适合目录+文件的配置组合
def GetCombination(SectionOne,OptionOne,SectionTwo,OptionTwo,typeOne='string',typeTwo = 'string'):
	resultOne = ReadConf(SectionOne,OptionOne,typeOne)
	resultTwo = ReadConf(SectionTwo,OptionTwo,typeTwo)
	if resultOne == None or resultTwo == None:
		return None
	else:
		return resultOne+resultTwo
	pass

def GetLibName():
	return GetCombination('app_env','LibDir','app_env','LibName')
def GetLogName():
	return GetCombination('app_env','LibDir','app_env','LibName')

def loadCfg(cfgname):
	if (not isinstance(cfgname,str)) or (len(cfgname) <= 0) :
		logging.error("传入参数有误")
		return 0
	cfgFile = GetCombination('app_env','CfgTransDef','trans_type',cfgname)
	logging.info(cfgFile)
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
				#logging.info(l)
				valueWay.append(l[0])
				valueWay.append(l[1])
				valueWay.append(l[2])
				#logging.info(valueWay)
				packDef.append(valueWay)
				'''
				valueSet = customizeFun.AutoSetFld(valueWay)
				if valueSet != None and len(valueSet) > 0:
					setPackageFlf(int(l[0]),valueSet)
				'''
	logging.info(packDef)
	return len(packDef)
	pass

packDef = []
	

ServerIp = ReadConf('app_env','ServerIp')
ServerPort = ReadConf('app_env','ServerPort','int')
packageDir = ReadConf('cfg_env','packageDir')
term8583 = ReadConf('cfg_env','term8583')
TimeOut = ReadConf('variables','TimeOut','int')
packageHeader = ReadConf('cfg_env','termHeader')
termid = ReadConf('termInfo','termid')
mid = ReadConf('termInfo','mid')
InsNo = ReadConf('termInfo','InsNo')
tmk = ReadConf('termInfo','tmk')
tak = ReadConf('termInfo','tak')
tpk = ReadConf('termInfo','tpk')

#ReadAllTransType()



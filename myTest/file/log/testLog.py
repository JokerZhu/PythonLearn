#!/usr/bin/python2.4
#-*- coding:utf8 -*-
import logging
import time

def GetLogFileName():

	T = time.localtime()
	localDate = '%04d' % T[0] + '-'+'%02d' % T[1] +'-' + '%02d' % T[2]
	return 'AliTransSerch.' + localDate + '.log'
	pass


logging.basicConfig(level=logging.DEBUG,
	format='%(asctime)s [%(filename)s][line:%(lineno)d] [%(process)d] [%(levelname)s] %(message)s',
       datefmt='[%Y%y%d%H%M%S]',
                filename=GetLogFileName() ,
                filemode='a+')


logging.debug('this is debug message')
logging.info('this is info message')
logging.warning('this is warning message')
logging.error('this is error message')



